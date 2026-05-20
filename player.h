#ifndef TYPES_H
#define TYPES_H

// ============================================================
//  Tipos y constantes globales del juego
// ============================================================

// Dimensiones del mapa
static const int COLS     = 30;
static const int ROWS     = 22;
static const int TILE     = 2;   // ancho visual por celda en la consola (chars)

// Tipos de tile
static const int T_EMPTY  = 0;
static const int T_WALL   = 1;
static const int T_FLOOR  = 2;
static const int T_DOOR   = 3;
static const int T_CLOSET = 4;

// IDs de habitaciones
static const int ROOM_NONE = 0;
static const int ROOM_1    = 1;  // sala principal (arriba-izquierda)
static const int ROOM_2    = 2;  // habitación media-izquierda
static const int ROOM_3    = 3;  // habitación abajo-izquierda
static const int ROOM_4    = 4;  // baño (centro)
static const int ROOM_5    = 5;  // dormitorio (arriba-derecha)
static const int ROOM_6    = 6;  // sala media-derecha
static const int ROOM_7    = 7;  // habitación final (abajo-derecha)

// IDs de ítems
static const int ITEM_NONE   = 0;
static const int ITEM_PAPER1 = 1;  // carta del novio (ROOM_2)
static const int ITEM_KEY    = 2;  // llave (ROOM_6)
static const int ITEM_CANDLE = 3;  // vela (ROOM_2)
static const int ITEM_PHONE  = 4;  // celular (ROOM_4)
static const int ITEM_PAPER2 = 5;  // carta final (ROOM_7)
static const int ITEM_MAX    = 5;

// Tipos de entidad
static const int ENT_PLAYER    = 0;
static const int ENT_BOYFRIEND = 1;
static const int ENT_CONSCIENCE = 2;

// Límites de arreglos estáticos (sin new/delete en game-loop)
static const int MAX_ITEMS   = 8;
static const int MAX_ENEMIES = 4;
static const int MAX_PATROL  = 16;

// Fases del juego
static const int PHASE_INTRO1   = 0;
static const int PHASE_INTRO2   = 1;
static const int PHASE_RUN      = 2;
static const int PHASE_PLAY     = 3;
static const int PHASE_READING  = 4;
static const int PHASE_DEAD     = 5;
static const int PHASE_WIN      = 6;

// Velocidades (frames entre movimientos)
static const int PLAYER_SPEED    = 1;   // cada cuántos ticks se mueve el jugador
static const int ENEMY_SPEED     = 4;   // ticks entre movimientos del novio
static const int CONSCIENCE_SPEED = 3;  // ticks entre movimientos de la conciencia
static const int CHASE_RANGE     = 7;   // distancia Manhattan para activar persecución

// Habitaciones maximas (indices 0-7)
static const int MAX_ROOMS = 8;

struct Vec2 {
    int r;
    int c;
};

#endif // TYPES_H
// (patch applied below — MAX_ROOMS ya disponible via define)
