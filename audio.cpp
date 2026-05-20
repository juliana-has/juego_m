#include "../include/game.h"
#include "../include/map.h"
#include "../include/input.h"
#include "../include/renderer.h"
#include "../include/audio.h"
#include "../include/narrative.h"
#include <cstring>
#include <cstdlib>

// ============================================================
//  game_init
// ============================================================
void game_init(GameState *gs) {
    map_init();
    player_init(&gs->player);
    enemy_init_all(gs->enemies, MAX_ENEMIES);
    items_init(gs->items, &gs->itemCount);

    gs->enemyCount       = MAX_ENEMIES;
    gs->phase            = PHASE_INTRO1;
    gs->introStep        = 0;
    gs->conscienceActive = false;
    gs->phoneRead        = false;
    gs->allCollected     = false;
    gs->readingItem      = ITEM_NONE;
    gs->tickCount        = 0;
    gs->lastRoom         = ROOM_NONE;
    gs->elapsedSeconds   = 0;
    gs->hudMessageTicks  = 0;
    gs->hudMessage[0]    = '\0';
    gs->bestScore        = score_load_best();
    gs->killedBy         = -1;

    for (int i = 0; i < MAX_ROOMS; i++) gs->roomVisited[i] = false;
}

// ============================================================
//  game_reset
// ============================================================
void game_reset(GameState *gs) {
    game_init(gs);
}

// ============================================================
//  game_is_over
// ============================================================
bool game_is_over(const GameState *gs) {
    return (gs->phase == PHASE_DEAD || gs->phase == PHASE_WIN);
}

// ---- Helper: mostrar mensaje en HUD ----
static void set_hud_msg(GameState *gs, const char *msg, int ticks) {
    int i = 0;
    while (msg[i] && i < 127) { gs->hudMessage[i] = msg[i]; i++; }
    gs->hudMessage[i]    = '\0';
    gs->hudMessageTicks  = ticks;
}

// ============================================================
//  game_handle_intro
// ============================================================
void game_handle_intro(GameState *gs) {
    render_screen(gs->phase, gs->introStep, gs->killedBy);
    render_wait_enter();

    if (gs->phase == PHASE_INTRO1) { gs->phase = PHASE_INTRO2; return; }
    if (gs->phase == PHASE_INTRO2) { gs->phase = PHASE_RUN;    return; }
    if (gs->phase == PHASE_RUN) {
        timer_start();
        gs->phase = PHASE_PLAY;
        return;
    }
    if (gs->phase == PHASE_DEAD) {
        game_reset(gs);
        return;
    }
    if (gs->phase == PHASE_WIN) {
        game_reset(gs);
        return;
    }
}

// ============================================================
//  game_tick
// ============================================================
void game_tick(GameState *gs) {
    if (gs->phase != PHASE_PLAY) return;

    gs->tickCount++;
    gs->elapsedSeconds = timer_elapsed_seconds();

    // Bajar contador del mensaje flotante
    if (gs->hudMessageTicks > 0) gs->hudMessageTicks--;

    // ---- Input ----
    char key = 0;
    if (input_kbhit()) key = input_getch();

    // ---- Movimiento del jugador ----
    int dr = 0, dc = 0;
    if (key == 'w' || key == 'W') dr = -1;
    if (key == 's' || key == 'S') dr =  1;
    if (key == 'a' || key == 'A') dc = -1;
    if (key == 'd' || key == 'D') dc =  1;

    if (dr != 0 || dc != 0) {
        bool moved = player_move(&gs->player, dr, dc);
        if (moved) audio_play(SND_STEP);
    }

    // ---- Recoger item (E) ----
    if (key == 'e' || key == 'E') {
        if (gs->player.inventoryItem == ITEM_NONE) {
            Item *nearby = items_find_nearby(
                gs->items, gs->itemCount,
                gs->player.pos.r, gs->player.pos.c);
            if (nearby) {
                items_collect(nearby);
                player_pick(&gs->player, nearby->id);
                audio_play(SND_PICK);
                // Mensaje narrativo al recoger
                const char *names[] = {"","la carta","la llave","la vela","el celular","la carta final"};
                static char pickMsg[64];
                int id = nearby->id;
                if (id >= 1 && id <= 5) {
                    const char *nm = names[id];
                    // Construir mensaje sin sprintf complejo
                    pickMsg[0] = 'R'; pickMsg[1] = 'e'; pickMsg[2] = 'c'; pickMsg[3] = 'o';
                    pickMsg[4] = 'g'; pickMsg[5] = 'i'; pickMsg[6] = 's'; pickMsg[7] = 't';
                    pickMsg[8] = 'e'; pickMsg[9] = ' ';
                    int j = 10;
                    for (int k = 0; nm[k] && j < 62; k++) pickMsg[j++] = nm[k];
                    pickMsg[j++] = '.'; pickMsg[j] = '\0';
                    set_hud_msg(gs, pickMsg, 80);
                }
            }
        }
    }

    // ---- Leer/usar item (R) ----
    if (key == 'r' || key == 'R') {
        int inv = gs->player.inventoryItem;
        if (inv != ITEM_NONE && items_get_content(inv)) {
            gs->phase = PHASE_READING;
            gs->readingItem = inv;

            audio_play(SND_READ);
            input_restore();
            render_read_item(inv);
            render_wait_enter();
            input_init();

            if (inv == ITEM_PHONE) {
                gs->phoneRead        = true;
                gs->conscienceActive = true;
                enemy_activate_conscience(gs->enemies, gs->enemyCount);
                set_hud_msg(gs, "Tu conciencia despierta...", 120);
            }

            player_drop(&gs->player);
            gs->phase       = PHASE_PLAY;
            gs->readingItem = ITEM_NONE;
        }
    }

    // ---- Soltar item (Q) ----
    if (key == 'q' || key == 'Q') {
        player_drop(&gs->player);
    }

    // ---- Entrar/salir del armario ----
    if (player_is_in_closet(&gs->player)) {
        if (gs->lastRoom != -1) {  // primera vez que entra
            audio_play(SND_HIDE);
            set_hud_msg(gs, "Aguanta la respiracion...", 60);
        }
    }

    // ---- Actualizar conteo de items ----
    int nCollected = items_count_collected(gs->items, gs->itemCount);
    gs->allCollected = (nCollected >= gs->itemCount);

    // ---- Cambio de habitacion: dialogo narrativo ----
    int currentRoom = map_get_room(gs->player.pos.r, gs->player.pos.c);
    if (currentRoom != gs->lastRoom && currentRoom != ROOM_NONE) {
        const char *dlg = narrative_room_dialogue(
            currentRoom, gs->roomVisited, gs->conscienceActive);
        if (dlg) set_hud_msg(gs, dlg, 120);
        gs->roomVisited[currentRoom] = true;
    }
    gs->lastRoom = currentRoom;

    // ---- Actualizar enemigos ----
    int minDist = 999;
    for (int i = 0; i < gs->enemyCount; i++) {
        enemy_update(&gs->enemies[i], gs->player.pos);
        if (gs->enemies[i].active) {
            int d = enemy_dist(gs->enemies[i].pos, gs->player.pos);
            if (d < minDist) minDist = d;
        }
    }
    audio_update(minDist);

    // Sonido de persecucion cuando el enemigo esta muy cerca
    if (minDist <= 3 && (gs->tickCount % 8 == 0)) {
        audio_play(SND_CHASE);
    }

    // ---- Colision con enemigos (derrota) ----
    if (!player_is_in_closet(&gs->player)) {
        int killer = enemy_who_caught(gs->enemies, gs->enemyCount, gs->player.pos);
        if (killer != -1) {
            audio_play(SND_CATCH);
            gs->killedBy = killer;
            gs->phase    = PHASE_DEAD;
            return;
        }
    }

    // ---- Victoria ----
    if (player_is_in_closet(&gs->player) && gs->allCollected) {
        int room = map_get_room(gs->player.pos.r, gs->player.pos.c);
        if (room == ROOM_7) {
            audio_play(SND_WIN);
            score_save_if_better(gs->elapsedSeconds);
            gs->bestScore = score_load_best();
            gs->phase = PHASE_WIN;
            return;
        }
    }
}
