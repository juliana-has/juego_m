#include "../include/renderer.h"
#include "../include/map.h"
#include "../include/input.h"
#include "../include/narrative.h"
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
static void clear_screen() { system("cls"); }
static void cursor_home()  { COORD c = {0,0}; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c); }
#else
static void clear_screen() { printf("\033[2J\033[H"); fflush(stdout); }
static void cursor_home()  { printf("\033[H"); fflush(stdout); }
#endif

void render_clear() { clear_screen(); }
void render_home()  { cursor_home(); }

// ============================================================
//  render_map
// ============================================================
void render_map(
    const Player *player,
    const Enemy  *enemies,
    int           enemyCount,
    const Item   *items,
    int           itemCount)
{
    DrawData dd;
    dd.entityCount = 0;
    dd.itemCount   = 0;

    EntityDraw *ep = &dd.entities[dd.entityCount++];
    ep->r      = player->pos.r;
    ep->c      = player->pos.c;
    ep->symbol = player->inCloset ? 'H' : '@';
    ep->active = true;

    for (int i = 0; i < enemyCount && dd.entityCount < MAX_ENEMIES + 1; i++) {
        const Enemy *e = &enemies[i];
        if (!e->active) continue;
        EntityDraw *ed = &dd.entities[dd.entityCount++];
        ed->r      = e->pos.r;
        ed->c      = e->pos.c;
        ed->symbol = (e->type == ENT_BOYFRIEND) ? 'B' : 'V';
        ed->active = true;
    }

    for (int i = 0; i < itemCount && dd.itemCount < MAX_ITEMS; i++) {
        ItemDraw *id2 = &dd.items[dd.itemCount++];
        id2->r         = items[i].pos.r;
        id2->c         = items[i].pos.c;
        id2->symbol    = items[i].symbol;
        id2->collected = items[i].collected;
    }

    map_draw(&dd);
}

// ============================================================
//  render_hud  (ahora recibe GameState datos extra)
// ============================================================
void render_hud(
    const Player *player,
    const Item   *items,
    int           itemCount,
    bool          conscienceActive,
    int           /*phase*/,
    int           elapsedSeconds,
    int           bestScore,
    const char   *hudMessage,
    int           hudMessageTicks)
{
    char timeBuf[8];
    timer_format(timeBuf, elapsedSeconds);

    printf("%s", Color::RESET);
    printf("------------------------------------------------------------------\n");

    // Timer y mejor marca
    printf(" %sTiempo: %s%s", Color::YELLOW, timeBuf, Color::RESET);
    if (bestScore < 9999) {
        char bestBuf[8];
        timer_format(bestBuf, bestScore);
        printf("   %sMejor: %s%s", Color::GREEN, bestBuf, Color::RESET);
    }
    printf("\n");

    // Inventario
    printf(" Inventario: ");
    if (player->inventoryItem != ITEM_NONE) {
        printf("%s%s%s", Color::YELLOW, items_get_name(player->inventoryItem), Color::RESET);
        printf("  [R] leer  [Q] soltar");
    } else {
        printf("%s(vacio)%s", Color::DIM, Color::RESET);
    }
    printf("\n");

    // Objetos recogidos
    printf(" Objetos: ");
    const int ids[5] = {ITEM_PAPER1, ITEM_KEY, ITEM_CANDLE, ITEM_PHONE, ITEM_PAPER2};
    for (int j = 0; j < 5; j++) {
        bool col = false;
        for (int i = 0; i < itemCount; i++)
            if (items[i].id == ids[j] && items[i].collected) col = true;
        if (col) printf("%s[x]%s", Color::GREEN, Color::RESET);
        else     printf("[ ]");
        printf(" %-14s", items_get_name(ids[j]));
    }
    printf("\n");

    // Mensaje narrativo flotante
    if (hudMessageTicks > 0 && hudMessage && hudMessage[0]) {
        printf(" %s> %s%s\n", Color::CYAN, hudMessage, Color::RESET);
    } else {
        printf("\n"); // mantener altura del HUD estable
    }

    // Estado especial
    if (player->inCloset)
        printf(" %s[ESCONDIDA — no respires]%s\n", Color::GREEN, Color::RESET);
    else if (conscienceActive)
        printf(" %sTu conciencia no te deja ir...%s\n", Color::MAGENTA, Color::RESET);
    else
        printf("\n");

    printf(" Controles: [WASD] mover  [E] recoger  [R] leer  [Q] soltar\n");
    printf(" Leyenda: %s@%s=Eli  %sB%s=Novio  %sV%s=Conciencia  %s?kip!%s=Objetos  C=Armario  +=Puerta\n",
        Color::CYAN,    Color::RESET,
        Color::RED,     Color::RESET,
        Color::MAGENTA, Color::RESET,
        Color::YELLOW,  Color::RESET);
    printf("------------------------------------------------------------------\n");
    fflush(stdout);
}

// ============================================================
//  render_screen — pantallas narrativas con typewriter
// ============================================================
void render_screen(int phase, int /*subStep*/, int killedBy) {
    render_clear();

    if (phase == PHASE_INTRO1) {
        printf("\n\n\n\n");
        printf("  %s", Color::RED);
        narrative_typewriter("  Alguna vez pensaste", 38);
        printf("\n\n  ");
        narrative_typewriter("  que moririas", 38);
        printf("\n\n  ");
        printf("%s", Color::BOLD);
        narrative_typewriter("  por amor?", 55);
        printf("%s\n\n\n\n", Color::RESET);
        printf("                    [ENTER]\n");
        fflush(stdout);
        return;
    }

    if (phase == PHASE_INTRO2) {
        printf("\n\n\n\n");
        printf("  %s", Color::WHITE);
        narrative_typewriter("  Pues Eli nunca lo penso.", 36);
        printf("\n\n  ");
        narrative_typewriter("  Nunca penso que la persona", 36);
        printf("\n  ");
        narrative_typewriter("  que decia amarla", 36);
        printf("\n\n  %s", Color::RED);
        narrative_typewriter("  pudiera ser de quien tuviera que huir.", 42);
        printf("%s\n\n\n\n", Color::RESET);
        printf("                    [ENTER]\n");
        fflush(stdout);
        return;
    }

    if (phase == PHASE_RUN) {
        printf("\n\n\n\n");
        printf("  %s", Color::DIM);
        narrative_typewriter("  Eli escucha sus pasos.", 32);
        printf("\n  ");
        narrative_typewriter("  Se acercan.", 32);
        printf("\n\n  %s", Color::RESET);
        narrative_typewriter("  Recoge todo lo que puedas.", 32);
        printf("\n  ");
        narrative_typewriter("  Necesitas pruebas de lo que viviste.", 32);
        printf("\n  ");
        narrative_typewriter("  Y necesitas llegar al armario.", 32);
        printf("\n\n  %s%s", Color::BOLD, Color::RED);
        narrative_typewriter("  CORRE.", 90);
        printf("%s\n\n\n\n", Color::RESET);
        printf("                    [ENTER]\n");
        fflush(stdout);
        return;
    }

    if (phase == PHASE_DEAD) {
        printf("\n\n\n\n");
        if (killedBy == ENT_CONSCIENCE) {
            // Muerte por la conciencia — paralisis, no violencia
            printf("  %s", Color::MAGENTA);
            printf("  . . . . . . . . . . . . . . . . . . . .\n\n");
            printf("  %s", Color::RESET);
            narrative_typewriter("  No pudiste seguir ignorandome.", 45);
            printf("\n\n  ");
            printf("%s", Color::DIM);
            narrative_typewriter("  Sabias que algo estaba mal.", 40);
            printf("\n  ");
            narrative_typewriter("  Lo sabias desde hace mucho tiempo.", 40);
            printf("\n\n  %s", Color::MAGENTA);
            narrative_typewriter("  Y eso tambien te destruye.", 50);
            printf("\n\n  %s", Color::DIM);
            narrative_typewriter("  Has muerto. Y no de amor.", 45);
        } else {
            // Muerte por el novio — violencia directa
            printf("  %s", Color::RED);
            printf("  # # # # # # # # # # # # # # # # # # # #\n\n");
            printf("  %s", Color::RESET);
            narrative_typewriter("  Te atrapo.", 55);
            printf("\n\n  ");
            printf("%s", Color::DIM);
            narrative_typewriter("  Como tantas otras veces.", 45);
            printf("\n  ");
            narrative_typewriter("  Siempre decia que era la ultima.", 45);
            printf("\n\n  %s", Color::RED);
            narrative_typewriter("  Has muerto.", 55);
            printf("\n  ");
            printf("%s", Color::DIM);
            narrative_typewriter("  Y no de amor.", 45);
        }
        printf("%s\n\n\n\n", Color::RESET);
        printf("                    [ENTER para intentar de nuevo]\n");
        fflush(stdout);
        return;
    }

    if (phase == PHASE_WIN) {
        printf("\n\n\n\n");
        printf("  %s", Color::DIM);
        narrative_typewriter("  Estas en el armario.", 36);
        printf("\n  ");
        narrative_typewriter("  Aguantas la respiracion.", 36);
        printf("\n  ");
        narrative_typewriter("  Lo escuchas pasar.", 36);
        printf("\n  ");
        narrative_typewriter("  Seguir de largo.", 36);
        printf("\n\n  %s", Color::RESET);
        narrative_typewriter("  Por primera vez en mucho tiempo", 36);
        printf("\n  ");
        narrative_typewriter("  sientes que puedes respirar.", 36);
        printf("\n\n  %s%s", Color::BOLD, Color::MAGENTA);
        narrative_typewriter("  El amor no duele.", 50);
        printf("\n  ");
        narrative_typewriter("  Y no deberia doler nunca.", 50);
        printf("%s\n\n\n\n", Color::RESET);
        printf("                    [ENTER]\n");
        fflush(stdout);
        return;
    }
}

// ============================================================
//  render_read_item
// ============================================================
void render_read_item(int itemId) {
    const char *content = items_get_content(itemId);
    if (!content) return;

    render_clear();
    printf("\n  %s[ Leyendo: %s ]%s\n", Color::YELLOW,
           items_get_name(itemId), Color::RESET);
    printf("  +-------------------------------------------------+\n");
    narrative_typewriter(content, 25);
    printf("  +-------------------------------------------------+\n\n");
    printf("  [ENTER para cerrar]\n");
    fflush(stdout);
}

void render_wait_enter() {
    input_getch_blocking();
}
