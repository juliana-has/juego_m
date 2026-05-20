#ifndef NARRATIVE_H
#define NARRATIVE_H

#include "types.h"

// ============================================================
//  Modulo narrativo
//  - Efecto typewriter para intros
//  - Dialogo contextual por habitacion
//  - Contador de tiempo con mejor marca
// ============================================================

static const int MAX_MSG_LEN  = 128;
static const int MAX_DIALOGUE = 32;

// Mensaje flotante en el HUD (dialogo de habitacion)
struct Message {
    char  text[MAX_MSG_LEN];
    int   ticksLeft;   // cuantos ticks sigue visible
    bool  active;
};

// ---- Typewriter ----

// Imprime un string caracter a caracter con pausa entre letras.
// delay_ms: milisegundos entre caracteres (recomendado 30-50)
void narrative_typewriter(const char *text, int delay_ms);

// Imprime una linea centrada con typewriter
void narrative_typewriter_centered(const char *text, int delay_ms, int width);

// ---- Diálogos por habitación ----

// Devuelve el dialogo que debe mostrarse al entrar a una habitacion.
// Retorna NULL si no hay dialogo nuevo para esa habitacion en este momento.
// visited: arreglo de bools indicando que habitaciones ya fueron visitadas
const char* narrative_room_dialogue(int roomId, const bool *visited,
                                    bool conscienceActive);

// ---- Contador de tiempo ----

// Inicializa el contador (llamar al inicio de PHASE_PLAY)
void timer_start();

// Devuelve los segundos transcurridos desde timer_start()
int  timer_elapsed_seconds();

// Devuelve los milisegundos transcurridos
long long timer_elapsed_ms();

// Formatea el tiempo como "mm:ss" en el buffer dado (minimo 6 chars)
void timer_format(char *buf, int elapsed_seconds);

// ---- Mejor marca (archivo) ----

// Carga la mejor marca desde "bestscore.txt". Retorna 9999 si no existe.
int  score_load_best();

// Guarda la marca si es mejor que la actual
void score_save_if_better(int seconds);

#endif // NARRATIVE_H
