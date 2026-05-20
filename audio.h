#ifndef INPUT_H
#define INPUT_H

// ============================================================
//  Modulo de entrada de teclado (no-bloqueante, sin Enter)
// ============================================================

// Configura la terminal en modo raw (sin buffering, sin echo)
void input_init();

// Restaura la terminal al modo normal
void input_restore();

// Retorna true si hay una tecla disponible para leer
bool input_kbhit();

// Lee un caracter sin bloquear (llama solo si input_kbhit() == true)
char input_getch();

// Lee un caracter bloqueante (para pantallas de intro/lectura)
char input_getch_blocking();

// Codigos de teclas especiales
static const char KEY_UP    = 'w';
static const char KEY_DOWN  = 's';
static const char KEY_LEFT  = 'a';
static const char KEY_RIGHT = 'd';
static const char KEY_PICK  = 'e';  // recoger item
static const char KEY_USE   = 'r';  // leer/usar item
static const char KEY_DROP  = 'q';  // soltar item
static const char KEY_ENTER = '\n';

#endif // INPUT_H
