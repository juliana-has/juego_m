#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "player.h"
#include "enemy.h"
#include "items.h"

// ============================================================
//  Modulo principal del juego — estado global
// ============================================================

struct GameState {
    Player player;
    Enemy  enemies[MAX_ENEMIES];
    Item   items[MAX_ITEMS];
    int    itemCount;
    int    enemyCount;
    int    phase;
    int    introStep;
    bool   conscienceActive;
    bool   phoneRead;
    bool   allCollected;
    int    readingItem;
    int    tickCount;

    // Nuevos campos: narrativa y tiempo
    bool   roomVisited[MAX_ROOMS];   // habitaciones ya visitadas
    int    lastRoom;                 // habitacion donde estaba el tick anterior
    int    elapsedSeconds;           // segundos jugados (para HUD)
    int    bestScore;                // mejor marca cargada de archivo
    char   hudMessage[128];          // mensaje flotante actual
    int    hudMessageTicks;          // cuantos ticks queda visible
    int    killedBy;                 // ENT_BOYFRIEND, ENT_CONSCIENCE, o -1
};

void game_init(GameState *gs);
void game_tick(GameState *gs);
void game_handle_intro(GameState *gs);
bool game_is_over(const GameState *gs);
void game_reset(GameState *gs);

#endif // GAME_H
