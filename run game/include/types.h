#pragma once

// ── Dimensiones del mundo ──────────────────────────────────────────────
static const int MAP_W          = 40;
static const int MAP_H          = 20;
static const int MAX_ROOMS      = 8;
static const int MAX_ENEMIES    = 6;
static const int MAX_ITEMS      = 8;
static const int MAX_LETTERS    = 6;
static const int INVENTORY_SIZE = 3;   // collar, carta, vela

// ── IDs de ítems coleccionables ────────────────────────────────────────
static const int ITEM_COLLAR = 0;
static const int ITEM_CARTA  = 1;
static const int ITEM_VELA   = 2;

// ── Tiles ──────────────────────────────────────────────────────────────
enum Tile {
    TILE_EMPTY = 0,
    TILE_WALL  = 1,
    TILE_FLOOR = 2,
    TILE_DOOR  = 3,
    TILE_EXIT  = 4,
    TILE_TREE  = 5,
    TILE_PATH  = 6,
};

// ── Fases del juego ────────────────────────────────────────────────────
enum Phase {
    PHASE_POEM_1    = 0,   // "Alguna vez pensaste que morirías de amor?"
    PHASE_POEM_2    = 1,   // "o si quiera pensaste..."
    PHASE_POEM_3    = 2,   // "como papá siempre dijo..."
    PHASE_INTRO     = 3,   // ¡CORRE!
    PHASE_HOUSE     = 4,   // casa del novio
    PHASE_FOREST    = 5,   // bosque
    PHASE_HOME      = 6,   // casa de ella
    PHASE_NOTE      = 7,   // papel en el suelo + aparece el morado
    PHASE_FREEZE    = 8,   // no puede moverse, recoge las 3 cosas
    PHASE_FLASHBACK = 9,   // flashback: el con alguien más (rojo)
    PHASE_ENDING    = 10,  // carta final
    PHASE_DEAD      = 11,
};

// ── Tipos de enemigo ───────────────────────────────────────────────────
enum EnemyKind {
    ENEMY_HIM    = 0,   // morado — figura mental del novio
    ENEMY_FRIEND = 1,   // amarillos — figuras mentales de los amigos
    ENEMY_OTHER  = 2,   // cafe (dim yellow) — la persona real con quien estaba el
};

// ── Estructuras ────────────────────────────────────────────────────────
struct Vec2 { int x, y; };

struct Player {
    Vec2 pos;
    int  inventory[INVENTORY_SIZE];  // IDs de ítems recogidos
    int  itemCount;
    bool alive;
    bool frozen;                     // PHASE_FREEZE: no puede moverse
};

struct Enemy {
    Vec2      pos;
    EnemyKind kind;
    bool      active;
    int       moveTimer;
    int       moveDelay;
};

struct Item {
    Vec2        pos;
    bool        onFloor;
    int         id;
    const char* name;
    const char* description;
};

struct Letter {
    Vec2        pos;
    bool        onFloor;
    const char* text;
    bool        read;
};

struct Room {
    Room() : x(0), y(0), w(0), h(0), active(false), exitTo({0,0}) {}
    Room(int px, int py, int pw, int ph, bool a, Vec2 e)
        : x(px), y(py), w(pw), h(ph), active(a), exitTo(e) {}
    int  x, y, w, h;
    bool active;
    Vec2 exitTo;
};
