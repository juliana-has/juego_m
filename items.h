#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"
#include "player.h"
#include "enemy.h"
#include "items.h"

// ============================================================
//  Modulo de renderizado en consola (ASCII)
// ============================================================

void render_clear();
void render_home();

void render_map(
    const Player *player,
    const Enemy  *enemies,
    int           enemyCount,
    const Item   *items,
    int           itemCount
);

// Dibuja la HUD debajo del mapa (incluye timer, mensaje narrativo)
void render_hud(
    const Player *player,
    const Item   *items,
    int           itemCount,
    bool          conscienceActive,
    int           phase,
    int           elapsedSeconds,
    int           bestScore,
    const char   *hudMessage,
    int           hudMessageTicks
);

void render_screen(int phase, int subStep, int killedBy = -1);
void render_read_item(int itemId);
void render_wait_enter();

// Codigos de color ANSI
namespace Color {
    const char * const RESET   = "\033[0m";
    const char * const RED     = "\033[31m";
    const char * const GREEN   = "\033[32m";
    const char * const YELLOW  = "\033[33m";
    const char * const BLUE    = "\033[34m";
    const char * const MAGENTA = "\033[35m";
    const char * const CYAN    = "\033[36m";
    const char * const WHITE   = "\033[37m";
    const char * const BOLD    = "\033[1m";
    const char * const BG_RED  = "\033[41m";
    const char * const DIM     = "\033[2m";
}

#endif // RENDERER_H
