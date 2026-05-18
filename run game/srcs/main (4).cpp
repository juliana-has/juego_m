#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#include "map.h"
#include "entity.h"
#include "renderer.h"

// ── Estado global (todo estático — sin new/delete) ─────────────────────
static Tile   gMap[MAP_H][MAP_W];
static Room   gRooms[MAX_ROOMS];
static Enemy  gEnemies[MAX_ENEMIES];
static Item   gItems[MAX_ITEMS];
static Letter gLetters[MAX_LETTERS];
static Player gPlayer;

static int   gRoomCount   = 0;
static int   gEnemyCount  = 0;
static int   gItemCount   = 0;
static int   gLetterCount = 0;
static Phase gPhase       = PHASE_POEM_1;

static bool gNoteShown       = false;   // papel en casa de ella
static bool gEnemySpawned    = false;   // el morado aparece tras la nota
static bool gShowingLetter   = false;
static const char* gLetterText = nullptr;

// ── Carga de fase ──────────────────────────────────────────────────────
static void loadPhase(Phase p) {
    gPhase          = p;
    gNoteShown      = false;
    gEnemySpawned   = false;
    gShowingLetter  = false;
    gLetterText     = nullptr;
    Vec2 start      = {0,0};

    switch (p) {
        case PHASE_HOUSE:
            buildHouseMap(gMap, gRooms, &gRoomCount,
                          gItems, &gItemCount,
                          gLetters, &gLetterCount,
                          &start, gEnemies, &gEnemyCount);
            break;
        case PHASE_FOREST:
            buildForestMap(gMap, gRooms, &gRoomCount,
                           &start, gEnemies, &gEnemyCount);
            for (int i=0;i<MAX_ITEMS;i++)   gItems[i].onFloor   = false;
            for (int i=0;i<MAX_LETTERS;i++) gLetters[i].onFloor = false;
            gItemCount = 0; gLetterCount = 0;
            break;
        case PHASE_HOME:
            buildHomeMap(gMap, gRooms, &gRoomCount,
                         &start, gEnemies, &gEnemyCount);
            gItemCount   = 0;
            gLetterCount = 0;
            break;
        default:
            break;
    }
    playerInit(&gPlayer, start);
    // Conservar inventario entre fases
    // (playerInit lo reinicia, así que restauramos count de fases previas)
}

// ── Conserva inventario al cambiar de fase ─────────────────────────────
static int  gSavedInv[INVENTORY_SIZE];
static int  gSavedCount = 0;

static void saveInventory() {
    gSavedCount = gPlayer.itemCount;
    for (int i=0; i<INVENTORY_SIZE; i++) gSavedInv[i] = gPlayer.inventory[i];
}

static void restoreInventory() {
    gPlayer.itemCount = gSavedCount;
    for (int i=0; i<INVENTORY_SIZE; i++) gPlayer.inventory[i] = gSavedInv[i];
}

// ── Utiles ─────────────────────────────────────────────────────────────
static bool playerOnExit() {
    return gMap[gPlayer.pos.y][gPlayer.pos.x] == TILE_EXIT;
}

static bool allItemsCollected() {
    for (int id=0; id<3; id++)
        if (!playerHasItem(&gPlayer, id)) return false;
    return true;
}

static Letter* letterUnderPlayer() {
    for (int i=0; i<gLetterCount; i++) {
        Letter* l = &gLetters[i];
        if (l->onFloor && !l->read &&
            l->pos.x == gPlayer.pos.x && l->pos.y == gPlayer.pos.y)
            return l;
    }
    return nullptr;
}

// ── Espera ENTER ───────────────────────────────────────────────────────
static void waitEnter() {
    nodelay(stdscr, FALSE);
    int c;
    do { c = getch(); } while (c != '\n' && c != KEY_ENTER && c != ' ');
    nodelay(stdscr, TRUE);
}

// ── Bucle principal ────────────────────────────────────────────────────
int main() {
    srand((unsigned)time(nullptr));
    rendererInit();

    // ── Secuencia de poemas ────────────────────────────────────────────
    for (int step = 0; step < 3; step++) {
        drawPoem(step);
        waitEnter();
    }

    // ── ¡CORRE! ────────────────────────────────────────────────────────
    drawIntro();
    waitEnter();

    loadPhase(PHASE_HOUSE);
    nodelay(stdscr, TRUE);

    bool running = true;

    while (running) {
        // ── Dibuja ─────────────────────────────────────────────────────
        clear();

        if (gPhase == PHASE_DEAD) {
            drawGameOver();
            int c = getch();
            if (c=='r'||c=='R') { loadPhase(PHASE_HOUSE); }
            else if (c=='q'||c=='Q') { running = false; }
            usleep(80000);
            continue;
        }

        if (gPhase == PHASE_FREEZE) {
            // No puede moverse; enemigos siguen acercándose
            drawMap(gMap, &gPlayer, gEnemies, gItems, gLetters, gPhase);
            drawHUD(&gPlayer, gPhase);
            refresh();

            enemiesUpdate(gEnemies, gEnemyCount, &gPlayer, gMap);

            if (enemiesHitPlayer(gEnemies, gEnemyCount, &gPlayer)) {
                // Muere → flashback
                nodelay(stdscr, FALSE);
                drawFlashback();
                waitEnter();
                drawEnding();
                // Espera Q
                int c2;
                do { c2 = getch(); } while (c2!='q'&&c2!='Q');
                running = false;
            }
            usleep(80000);
            continue;
        }

        drawMap(gMap, &gPlayer, gEnemies, gItems, gLetters, gPhase);
        drawHUD(&gPlayer, gPhase);

        if (gShowingLetter && gLetterText) {
            drawLetter(gLetterText);
        }

        refresh();

        // ── Input ──────────────────────────────────────────────────────
        int ch = getch();

        if (gShowingLetter) {
            if (ch != ERR) {
                gShowingLetter = false;
                gLetterText    = nullptr;
            }
            usleep(50000);
            continue;
        }

        int dx=0, dy=0;
        switch (ch) {
            case 'w': case 'W': case KEY_UP:    dy=-1; break;
            case 's': case 'S': case KEY_DOWN:  dy= 1; break;
            case 'a': case 'A': case KEY_LEFT:  dx=-1; break;
            case 'd': case 'D': case KEY_RIGHT: dx= 1; break;
            case 'e': case 'E':
                playerPickup(&gPlayer, gItems, gItemCount);
                break;
            case 'f': case 'F': {
                Letter* l = letterUnderPlayer();
                if (l) {
                    l->read        = true;
                    gShowingLetter = true;
                    gLetterText    = l->text;
                }
                break;
            }
            case 'q': case 'Q': running = false; break;
        }

        if (dx||dy) playerMove(&gPlayer, dx, dy, gMap);

        // ── Lógica de fase ─────────────────────────────────────────────
        enemiesUpdate(gEnemies, gEnemyCount, &gPlayer, gMap);

        if (enemiesHitPlayer(gEnemies, gEnemyCount, &gPlayer)) {
            gPhase = PHASE_DEAD;
            usleep(50000);
            continue;
        }

        // Transición entre fases
        if (playerOnExit()) {
            if (gPhase == PHASE_HOUSE) {
                saveInventory();
                loadPhase(PHASE_FOREST);
                restoreInventory();
            } else if (gPhase == PHASE_FOREST) {
                saveInventory();
                loadPhase(PHASE_HOME);
                restoreInventory();
            }
        }

        // ── Lógica especial de PHASE_HOME ─────────────────────────────
        if (gPhase == PHASE_HOME) {

            // 1. Mostrar nota + aparición del morado (una sola vez)
            if (!gNoteShown) {
                gNoteShown = true;
                nodelay(stdscr, FALSE);
                drawHomeNote();
                waitEnter();
                nodelay(stdscr, TRUE);
            }

            // 2. Activar enemigo morado tras la nota
            if (!gEnemySpawned) {
                gEnemySpawned = true;
                enemyInit(&gEnemies[0], {20, 10}, ENEMY_HIM,    5);
                enemyInit(&gEnemies[1], {30, 15}, ENEMY_FRIEND, 3);
                enemyInit(&gEnemies[2], {10, 2},  ENEMY_FRIEND, 3);
                gEnemyCount = 3;
            }

            // 3. Si recogió los 3 ítems → FREEZE
            if (allItemsCollected() && !gPlayer.frozen) {
                gPlayer.frozen = true;
                gPhase = PHASE_FREEZE;
            }
        }

        usleep(50000);  // ~20 FPS
    }

    rendererShutdown();
    return 0;
}
