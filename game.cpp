#include "../include/narrative.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
static void sleep_ms_narr(int ms) { Sleep(ms); }
static long long get_ms() {
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (long long)(cnt.QuadPart * 1000LL / freq.QuadPart);
}
#else
#include <unistd.h>
#include <time.h>
static void sleep_ms_narr(int ms) { usleep(ms * 1000); }
static long long get_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}
#endif

// ============================================================
//  Typewriter
// ============================================================
void narrative_typewriter(const char *text, int delay_ms) {
    for (int i = 0; text[i] != '\0'; i++) {
        putchar(text[i]);
        fflush(stdout);
        if (text[i] == '.' || text[i] == '?' || text[i] == '!')
            sleep_ms_narr(delay_ms * 6);
        else if (text[i] == ',' || text[i] == ';' || text[i] == ':')
            sleep_ms_narr(delay_ms * 3);
        else if (text[i] == '\n')
            sleep_ms_narr(delay_ms * 5);
        else if (text[i] == ' ')
            sleep_ms_narr(delay_ms);
        else
            sleep_ms_narr(delay_ms);
    }
}

void narrative_typewriter_centered(const char *text, int delay_ms, int width) {
    int len = (int)strlen(text);
    int pad = (width - len) / 2;
    for (int i = 0; i < pad; i++) putchar(' ');
    narrative_typewriter(text, delay_ms);
}

// ============================================================
//  Dialogos — primera visita a cada habitacion
// ============================================================
static const char *ROOM_FIRST_VISIT[8] = {
    // 0 — ninguna
    "",
    // 1 — sala principal (donde empieza todo)
    "Esta sala. Cuantas veces te convenciste de que todo estaba bien aqui.",
    // 2 — hab media-izquierda (carta del novio, vela)
    "Huele a vela apagada. A noches esperandolo con miedo de que llegara.",
    // 3 — hab abajo-izquierda
    "Aqui fue la primera vez que te pidio perdon. Lo creiste.",
    // 4 — bano (celular)
    "Cuantas veces lloraste aqui con el agua abierta para que no te oyera.",
    // 5 — dormitorio
    "Su cuarto. Todavia recuerdas que dias te quedabas quieta, sin respirar.",
    // 6 — sala media-derecha (llave)
    "La llave. Siempre decia que era por si acaso. Por si acaso tu intentabas irte.",
    // 7 — habitacion final
    "El armario. Si llegas... si llegas puedes salir. Todavia puedes salir."
};

// ============================================================
//  Dialogos — segunda visita (sin conciencia activa)
// ============================================================
static const char *ROOM_SECOND_VISIT[8] = {
    "",
    "Por que seguiste aqui tanto tiempo. Por que.",
    "Recuerdas doblar ese papel mil veces. Guardarlo. Volver a leerlo.",
    "Te dijo que eras demasiado sensible. Y le creiste.",
    "Tu papa llamaba todos los dias. Nunca contestaste.",
    "Una noche el durmio tranquilo. Tu no pegaste los ojos.",
    "Esa llave nunca fue tuya de verdad.",
    "Ya casi. No te detengas ahora."
};

// ============================================================
//  Dialogos — conciencia (se muestran cuando esta activa)
//  Estan ordenados para contar una historia, no son aleatorios.
// ============================================================
static const char *CONSCIENCE_LINES[] = {
    "Eli. Soy yo. La parte de ti que el no pudo callar.",
    "Recuerdas cuando dejaste de llamarle a tus amigas? El te lo pidio.",
    "Eso que sientes en el pecho no es amor. El amor no aprieta asi.",
    "Cuantas veces dijiste 'es la ultima' y no lo fue.",
    "El te hacia sentir que sin el no eras nada. Mentira.",
    "Tu papa sigue esperando. Todavia puedes llamarle.",
    "No eres exagerada. No eres demasiado. Eres suficiente.",
    "Corre. No mires atras. Esta vez de verdad."
};
static const int N_CONSCIENCE = 8;
static int gConscienceIdx = 0;

// ============================================================
//  narrative_room_dialogue
// ============================================================
const char* narrative_room_dialogue(int roomId, const bool *visited,
                                    bool conscienceActive) {
    if (roomId < 0 || roomId > 7) return 0;

    if (!visited[roomId]) {
        const char *msg = ROOM_FIRST_VISIT[roomId];
        if (msg && msg[0]) return msg;
        return 0;
    }

    // Segunda visita con conciencia activa: frases de la conciencia
    if (conscienceActive) {
        if (gConscienceIdx < N_CONSCIENCE) {
            const char *line = CONSCIENCE_LINES[gConscienceIdx];
            gConscienceIdx++;
            return line;
        }
        return 0;
    }

    // Segunda visita sin conciencia
    const char *msg2 = ROOM_SECOND_VISIT[roomId];
    if (msg2 && msg2[0]) return msg2;
    return 0;
}

// ============================================================
//  Timer
// ============================================================
static long long gStartMs = 0;

void timer_start() {
    gStartMs = get_ms();
}

long long timer_elapsed_ms() {
    if (gStartMs == 0) return 0;
    return get_ms() - gStartMs;
}

int timer_elapsed_seconds() {
    return (int)(timer_elapsed_ms() / 1000LL);
}

void timer_format(char *buf, int elapsed_seconds) {
    int m = elapsed_seconds / 60;
    int s = elapsed_seconds % 60;
    buf[0] = '0' + (m / 10);
    buf[1] = '0' + (m % 10);
    buf[2] = ':';
    buf[3] = '0' + (s / 10);
    buf[4] = '0' + (s % 10);
    buf[5] = '\0';
}

// ============================================================
//  Mejor marca
// ============================================================
static const char *SCORE_FILE = "bestscore.txt";

int score_load_best() {
    FILE *f = fopen(SCORE_FILE, "r");
    if (!f) return 9999;
    int val = 9999;
    fscanf(f, "%d", &val);
    fclose(f);
    return val;
}

void score_save_if_better(int seconds) {
    int best = score_load_best();
    if (seconds < best) {
        FILE *f = fopen(SCORE_FILE, "w");
        if (f) {
            fprintf(f, "%d\n", seconds);
            fclose(f);
        }
    }
}
