// ============================================================
//  audio.cpp — Sonido procedural con miniaudio (header-only)
//
//  miniaudio.h debe estar en la raiz del proyecto.
//  Descargalo de: https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
//
//  Si no existe, el modulo compila igual pero sin sonido.
// ============================================================

#include "../include/audio.h"
#include <cmath>
#include <cstring>
#include <cstdio>

// Intentar incluir miniaudio solo si existe
#if defined(__has_include)
  #if __has_include("miniaudio.h")
    #define MA_NO_ENCODING
    #define MA_NO_DECODING
    #define MINIAUDIO_IMPLEMENTATION
    #include "miniaudio.h"
    #define AUDIO_AVAILABLE 1
  #else
    #define AUDIO_AVAILABLE 0
  #endif
#else
  // Compiladores sin __has_include: intentamos incluir directamente
  // Si falla, el usuario debe agregar -DAUDIO_AVAILABLE=0
  #ifndef AUDIO_AVAILABLE
    #define AUDIO_AVAILABLE 0
  #endif
#endif

#if AUDIO_AVAILABLE

// ============================================================
//  Estado interno del sintetizador
// ============================================================
static const int   SAMPLE_RATE  = 44100;
static const int   CHANNELS     = 1;
static const float TWO_PI       = 6.28318530718f;

struct SynthState {
    // Oscilador principal
    float phase;
    float freq;
    float amplitude;
    int   samplesLeft;   // cuantas muestras quedan del evento actual
    int   currentEvent;

    // Latido continuo
    float heartPhase;
    float heartFreq;
    bool  heartActive;
    int   heartTimer;    // muestras hasta el proximo golpe

    // Distancia al enemigo (para modular el latido)
    int   enemyDist;
};

static SynthState gSynth;
static ma_device  gDevice;
static bool       gAudioReady = false;

// ============================================================
//  Callback de audio (se llama desde hilo de audio)
// ============================================================
static void data_callback(ma_device *pDevice, void *pOutput,
                          const void *pInput, ma_uint32 frameCount)
{
    (void)pDevice;
    (void)pInput;

    float *out = (float *)pOutput;
    SynthState *s = &gSynth;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        float sample = 0.0f;

        // ---- Evento de sonido activo ----
        if (s->samplesLeft > 0) {
            s->samplesLeft--;
            float t = (float)s->samplesLeft / (float)SAMPLE_RATE;
            float env = (float)s->samplesLeft / (float)(SAMPLE_RATE / 4);
            if (env > 1.0f) env = 1.0f;

            switch (s->currentEvent) {

                case SND_STEP: {
                    // Click corto de paso (ruido blanco breve)
                    float noise = ((float)(s->samplesLeft % 7) / 7.0f) - 0.5f;
                    sample += noise * env * 0.15f;
                    break;
                }
                case SND_CHASE: {
                    // Tono agudo pulsante
                    s->phase += TWO_PI * 880.0f / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    float sq = (s->phase < 3.14159f) ? 1.0f : -1.0f;
                    sample += sq * env * 0.2f;
                    break;
                }
                case SND_CATCH: {
                    // Descenso brusco de tono (derrota)
                    float freq = 400.0f - (1.0f - env) * 380.0f;
                    s->phase += TWO_PI * freq / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    sample += sinf(s->phase) * env * 0.4f;
                    break;
                }
                case SND_PICK: {
                    // Dos notas rapidas ascendentes
                    float freq2 = (s->samplesLeft > SAMPLE_RATE / 16) ? 523.0f : 659.0f;
                    s->phase += TWO_PI * freq2 / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    sample += sinf(s->phase) * env * 0.25f;
                    break;
                }
                case SND_READ: {
                    // Tono suave y tembloroso (papel)
                    s->phase += TWO_PI * 330.0f / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    float vib = 1.0f + 0.02f * sinf(TWO_PI * 6.0f * t);
                    sample += sinf(s->phase * vib) * env * 0.2f;
                    break;
                }
                case SND_HIDE: {
                    // Tono grave, alivio
                    s->phase += TWO_PI * 196.0f / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    sample += sinf(s->phase) * env * 0.3f;
                    break;
                }
                case SND_WIN: {
                    // Acorde ascendente
                    float notes[3] = {261.63f, 329.63f, 392.0f};
                    float idx_f = (float)(SAMPLE_RATE / 4 - s->samplesLeft) / (SAMPLE_RATE / 12.0f);
                    int idx = (int)idx_f;
                    if (idx > 2) idx = 2;
                    s->phase += TWO_PI * notes[idx] / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    sample += sinf(s->phase) * env * 0.35f;
                    break;
                }
                case SND_HEARTBEAT: {
                    // Latido doble (lub-dub)
                    s->phase += TWO_PI * 60.0f / SAMPLE_RATE;
                    if (s->phase > TWO_PI) s->phase -= TWO_PI;
                    sample += sinf(s->phase) * env * 0.3f;
                    break;
                }
                default: break;
            }
        }

        // ---- Latido continuo de fondo ----
        if (s->heartActive) {
            s->heartTimer--;
            if (s->heartTimer <= 0) {
                // Intervalo segun distancia: mas cerca = mas rapido
                int bpm = 60;
                if (s->enemyDist <= 3)       bpm = 160;
                else if (s->enemyDist <= 6)  bpm = 120;
                else if (s->enemyDist <= 10) bpm = 90;
                int interval = (int)((60.0f / bpm) * SAMPLE_RATE / 2);
                s->heartTimer = interval;

                float vol = (s->enemyDist <= 10) ? 0.08f : 0.02f;
                // Pulso corto grave
                s->heartPhase = 0.0f;
                sample += sinf(s->heartPhase) * vol;
            }
            // Generar muestra del pulso activo (decay rapido)
            s->heartPhase += TWO_PI * 80.0f / SAMPLE_RATE;
            float henv = expf(-s->heartPhase * 0.3f);
            sample += sinf(s->heartPhase) * henv * 0.06f;
        }

        // Clamp
        if (sample >  1.0f) sample =  1.0f;
        if (sample < -1.0f) sample = -1.0f;

        *out++ = sample;
    }
}

// ============================================================
//  API publica
// ============================================================
bool audio_init() {
    memset(&gSynth, 0, sizeof(gSynth));
    gSynth.heartActive = true;
    gSynth.heartTimer  = SAMPLE_RATE / 2;
    gSynth.enemyDist   = 20;

    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format   = ma_format_f32;
    cfg.playback.channels = CHANNELS;
    cfg.sampleRate        = SAMPLE_RATE;
    cfg.dataCallback      = data_callback;
    cfg.pUserData         = &gSynth;

    if (ma_device_init(NULL, &cfg, &gDevice) != MA_SUCCESS) {
        fprintf(stderr, "[audio] No se pudo inicializar el dispositivo de audio.\n");
        return false;
    }
    if (ma_device_start(&gDevice) != MA_SUCCESS) {
        ma_device_uninit(&gDevice);
        return false;
    }
    gAudioReady = true;
    return true;
}

void audio_shutdown() {
    if (gAudioReady) {
        ma_device_uninit(&gDevice);
        gAudioReady = false;
    }
}

void audio_play(int soundEvent) {
    if (!gAudioReady) return;
    int dur = SAMPLE_RATE / 4; // 250ms por defecto
    if (soundEvent == SND_STEP)      dur = SAMPLE_RATE / 16;
    if (soundEvent == SND_CATCH)     dur = SAMPLE_RATE / 2;
    if (soundEvent == SND_WIN)       dur = SAMPLE_RATE;
    if (soundEvent == SND_HEARTBEAT) dur = SAMPLE_RATE / 8;

    gSynth.currentEvent = soundEvent;
    gSynth.samplesLeft  = dur;
    gSynth.phase        = 0.0f;
}

void audio_update(int dist) {
    if (!gAudioReady) return;
    gSynth.enemyDist = dist;
}

// ============================================================
//  Sin miniaudio: stubs vacios
// ============================================================
#else

bool audio_init()              { return false; }
void audio_shutdown()          {}
void audio_play(int)           {}
void audio_update(int)         {}

#endif // AUDIO_AVAILABLE
