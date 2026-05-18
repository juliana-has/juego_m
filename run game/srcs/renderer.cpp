#include "renderer.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

// ── Pares de color ─────────────────────────────────────────────────────
#define C_WALL      1
#define C_FLOOR     2
#define C_PLAYER    3   // azul
#define C_ENEMY_HIM 4   // morado
#define C_ENEMY_FR  5   // amarillo
#define C_ENEMY_OTH 6   // rojo (flashback)
#define C_ITEM      7   // cyan
#define C_EXIT      8   // verde
#define C_TREE      9   // verde oscuro
#define C_PATH      10  // amarillo tenue
#define C_TEXT      11  // blanco
#define C_LETTER    12  // blanco brillante
#define C_RED       13  // rojo para game over / énfasis
#define C_DOOR      14  // amarillo
#define C_POEM      15  // blanco tenue
#define C_REAL_HIM  16  // rojo — el novio REAL (flashback)

void rendererInit() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);
    curs_set(0);
    start_color();
    use_default_colors();

    init_pair(C_WALL,      COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_FLOOR,     COLOR_BLACK,   COLOR_BLACK);
    init_pair(C_PLAYER,    COLOR_BLUE,    COLOR_BLACK);
    init_pair(C_ENEMY_HIM, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(C_ENEMY_FR,  COLOR_YELLOW,  COLOR_BLACK);
    init_pair(C_ENEMY_OTH, COLOR_YELLOW,  COLOR_BLACK);  // sin A_BOLD = cafe/marron
    init_pair(C_ITEM,      COLOR_CYAN,    COLOR_BLACK);
    init_pair(C_EXIT,      COLOR_GREEN,   COLOR_BLACK);
    init_pair(C_TREE,      COLOR_GREEN,   COLOR_BLACK);
    init_pair(C_PATH,      COLOR_YELLOW,  COLOR_BLACK);
    init_pair(C_TEXT,      COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_LETTER,    COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_RED,       COLOR_RED,     COLOR_BLACK);
    init_pair(C_DOOR,      COLOR_YELLOW,  COLOR_BLACK);
    init_pair(C_POEM,      COLOR_WHITE,   COLOR_BLACK);
    init_pair(C_REAL_HIM,  COLOR_RED,     COLOR_BLACK);
}

void rendererShutdown() { endwin(); }
void clearScreen()       { clear(); }

// ── Helper: texto centrado ─────────────────────────────────────────────
static void printCentered(int row, const char* text, int colorPair, int attrs) {
    int cols = getmaxx(stdscr);
    int len  = (int)strlen(text);
    int col  = (cols - len) / 2;
    if (col < 0) col = 0;
    attron(COLOR_PAIR(colorPair) | attrs);
    mvprintw(row, col, "%s", text);
    attroff(COLOR_PAIR(colorPair) | attrs);
}

// ── Pantallas de poema ─────────────────────────────────────────────────
void drawPoem(int step) {
    clear();
    int rows = getmaxy(stdscr);
    int mid  = rows / 2;

    const char* lines[] = {
        "Alguna vez pensaste que morias de amor?",
        "...o siquiera pensaste que lograrias amar tanto\ncomo para destruirte?",
        "Como papa siempre dijo:\n\"el que ama mas siempre se queda con el dolor\"\n...y se que tal vez yo no sea la excepcion."
    };

    // Dibuja línea por línea (maneja \n)
    const char* text = lines[step];
    int row = mid - 1;
    int col = 4;
    int lineCol = col;
    attron(COLOR_PAIR(C_POEM) | A_BOLD);
    const char* p = text;
    while (*p) {
        if (*p == '\n') { row++; lineCol = col; }
        else mvaddch(row, lineCol++, (unsigned char)*p);
        p++;
    }
    attroff(COLOR_PAIR(C_POEM) | A_BOLD);

    printCentered(rows - 3, "[ Presiona ENTER para continuar ]", C_TEXT, A_DIM);
    refresh();
}

// ── Pantalla ¡CORRE! ───────────────────────────────────────────────────
void drawIntro() {
    clear();
    int rows = getmaxy(stdscr);
    int mid  = rows / 2;
    printCentered(mid - 1, "!CORRE!", C_RED, A_BOLD);
    printCentered(mid + 2,  "[ Presiona ENTER ]", C_TEXT, A_DIM);
    refresh();
}

// ── Mapa de juego ──────────────────────────────────────────────────────
void drawMap(const Tile   map[MAP_H][MAP_W],
             const Player* player,
             const Enemy   enemies[MAX_ENEMIES],
             const Item    items[MAX_ITEMS],
             const Letter  letters[MAX_LETTERS],
             Phase         phase) {

    for (int r = 0; r < MAP_H; r++) {
        for (int c = 0; c < MAP_W; c++) {
            int sr = r + 1;
            int sc = c * 2;
            switch (map[r][c]) {
                case TILE_WALL:
                    attron(COLOR_PAIR(C_WALL) | A_BOLD);
                    mvprintw(sr, sc, "##");
                    attroff(COLOR_PAIR(C_WALL) | A_BOLD);
                    break;
                case TILE_FLOOR:
                    mvprintw(sr, sc, "  ");
                    break;
                case TILE_DOOR:
                    attron(COLOR_PAIR(C_DOOR) | A_BOLD);
                    mvprintw(sr, sc, "||");
                    attroff(COLOR_PAIR(C_DOOR) | A_BOLD);
                    break;
                case TILE_EXIT:
                    attron(COLOR_PAIR(C_EXIT) | A_BOLD);
                    mvprintw(sr, sc, ">>");
                    attroff(COLOR_PAIR(C_EXIT) | A_BOLD);
                    break;
                case TILE_TREE:
                    attron(COLOR_PAIR(C_TREE));
                    mvprintw(sr, sc, "%%");
                    attroff(COLOR_PAIR(C_TREE));
                    break;
                case TILE_PATH:
                    attron(COLOR_PAIR(C_PATH));
                    mvprintw(sr, sc, ". ");
                    attroff(COLOR_PAIR(C_PATH));
                    break;
                default:
                    mvprintw(sr, sc, "  ");
                    break;
            }
        }
    }

    // Ítems
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (!items[i].onFloor) continue;
        attron(COLOR_PAIR(C_ITEM) | A_BOLD);
        mvprintw(items[i].pos.y + 1, items[i].pos.x * 2, "[]");
        attroff(COLOR_PAIR(C_ITEM) | A_BOLD);
    }

    // Cartas/notas
    for (int i = 0; i < MAX_LETTERS; i++) {
        if (!letters[i].onFloor || letters[i].read) continue;
        attron(COLOR_PAIR(C_LETTER) | A_BOLD);
        mvprintw(letters[i].pos.y + 1, letters[i].pos.x * 2, "~~");
        attroff(COLOR_PAIR(C_LETTER) | A_BOLD);
    }

    // Enemigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        const Enemy* e = &enemies[i];
        if (!e->active) continue;
        if (e->kind == ENEMY_HIM) {
            attron(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
            mvprintw(e->pos.y + 1, e->pos.x * 2, "EL");
            attroff(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
        } else if (e->kind == ENEMY_FRIEND) {
            attron(COLOR_PAIR(C_ENEMY_FR) | A_BOLD);
            mvprintw(e->pos.y + 1, e->pos.x * 2, "AM");
            attroff(COLOR_PAIR(C_ENEMY_FR) | A_BOLD);
        } else {
            // ENEMY_OTHER: cafe (amarillo oscuro sin bold), solo en flashback
            attron(COLOR_PAIR(C_ENEMY_OTH));
            mvprintw(e->pos.y + 1, e->pos.x * 2, "OT");
            attroff(COLOR_PAIR(C_ENEMY_OTH));
        }
    }

    // Jugadora — azul
    attron(COLOR_PAIR(C_PLAYER) | A_BOLD);
    mvprintw(player->pos.y + 1, player->pos.x * 2, "@");
    attroff(COLOR_PAIR(C_PLAYER) | A_BOLD);

    // Indicador FREEZE
    if (player->frozen) {
        int rows = getmaxy(stdscr);
        attron(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
        mvprintw(MAP_H + 2, 2, "No puedes moverte...");
        attroff(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
        (void)rows;
    }
}

// ── HUD ────────────────────────────────────────────────────────────────
void drawHUD(const Player* player, Phase phase) {
    int rows = getmaxy(stdscr);

    // Nombres de fase
    const char* phaseNames[] = {"","","","","Casa de el","Bosque","Tu casa","Tu casa","Tu casa","Recuerdo","",""};
    attron(COLOR_PAIR(C_TEXT));
    mvprintw(rows - 3, 0, " [WASD] Mover  [E] Recoger  [F] Leer  | %s", phaseNames[phase]);

    // Inventario (3 espacios)
    const char* slotNames[] = {"collar","carta","vela"};
    mvprintw(rows - 2, 0, " Inventario: ");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        bool has = false;
        for (int j = 0; j < player->itemCount; j++)
            if (player->inventory[j] == i) { has = true; break; }
        if (has) {
            attron(COLOR_PAIR(C_ITEM) | A_BOLD);
            printw("[%s]", slotNames[i]);
            attroff(COLOR_PAIR(C_ITEM) | A_BOLD);
        } else {
            attron(COLOR_PAIR(C_TEXT) | A_DIM);
            printw("[   ]");
            attroff(COLOR_PAIR(C_TEXT) | A_DIM);
        }
        printw(" ");
    }

    // Leyenda
    int cols = getmaxx(stdscr);
    attron(COLOR_PAIR(C_PLAYER)    | A_BOLD); mvprintw(rows-2, cols-32, "@=tu ");
    attroff(COLOR_PAIR(C_PLAYER)   | A_BOLD);
    attron(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD); mvprintw(rows-2, cols-27, "EL=el ");
    attroff(COLOR_PAIR(C_ENEMY_HIM)| A_BOLD);
    attron(COLOR_PAIR(C_ENEMY_FR)  | A_BOLD); mvprintw(rows-2, cols-21, "AM=amigo ");
    attroff(COLOR_PAIR(C_ENEMY_FR) | A_BOLD);
    attron(COLOR_PAIR(C_EXIT)      | A_BOLD); mvprintw(rows-2, cols-12, ">>=salida");
    attroff(COLOR_PAIR(C_EXIT)     | A_BOLD);

    attroff(COLOR_PAIR(C_TEXT));
}

// ── Cuadro de mensaje flotante ─────────────────────────────────────────
void drawMessage(const char* msg) {
    int rows = getmaxy(stdscr);
    int cols = getmaxx(stdscr);
    int len  = (int)strlen(msg);
    int bx   = (cols - len - 4) / 2;
    int by   = rows / 2 - 1;

    attron(COLOR_PAIR(C_TEXT) | A_BOLD);
    mvprintw(by,     bx, "+"); for(int i=0;i<len+2;i++) mvprintw(by,     bx+1+i, "-"); mvprintw(by,     bx+len+3, "+");
    mvprintw(by + 1, bx, "| %s |", msg);
    mvprintw(by + 2, bx, "+"); for(int i=0;i<len+2;i++) mvprintw(by + 2, bx+1+i, "-"); mvprintw(by + 2, bx+len+3, "+");
    attroff(COLOR_PAIR(C_TEXT) | A_BOLD);
    refresh();
}

// ── Carta / nota en pantalla completa ─────────────────────────────────
void drawLetter(const char* text) {
    clear();
    int rows = getmaxy(stdscr);

    attron(COLOR_PAIR(C_ITEM) | A_BOLD);
    mvprintw(2, 4, "--- Encontraste algo en el suelo ---");
    attroff(COLOR_PAIR(C_ITEM) | A_BOLD);

    attron(COLOR_PAIR(C_LETTER));
    int row = 5, col = 8, lineCol = col;
    const char* p = text;
    while (*p) {
        if (*p == '\n') { row++; lineCol = col; }
        else mvaddch(row, lineCol++, (unsigned char)*p);
        p++;
    }
    attroff(COLOR_PAIR(C_LETTER));

    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(rows - 3, 4, "[ Presiona cualquier tecla para continuar ]");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh();
}

// ── Nota en la casa de ella (el papel + aparición morada) ──────────────
void drawHomeNote() {
    clear();
    int rows = getmaxy(stdscr);
    int mid  = rows / 2;

    attron(COLOR_PAIR(C_TEXT) | A_BOLD);
    mvprintw(mid - 3, 6, "Hay un papel en el suelo.");
    mvprintw(mid - 1, 6, "\"Podemos arreglarlo, lo se.");
    mvprintw(mid,     6, " Se que muy en el fondo aun me amas.\"");
    attroff(COLOR_PAIR(C_TEXT) | A_BOLD);

    usleep(2500000);

    // Aparece el morado de la nada
    attron(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
    mvprintw(mid + 3, 6, "EL: \"Sabes que esto no es facil para mi.\"");
    attroff(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);

    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(rows - 3, 4, "[ Presiona ENTER para continuar ]");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh();
}

// ── Flashback ─────────────────────────────────────────────────────────
void drawFlashback() {
    clear();
    int rows = getmaxy(stdscr);
    int cols = getmaxx(stdscr);
    int mid  = rows / 2;

    // Parpadeo del morado antes de desvanecerse
    for (int i = 0; i < 3; i++) {
        attron(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
        mvprintw(mid, (cols-12)/2, "[ recuerdo ]");
        attroff(COLOR_PAIR(C_ENEMY_HIM) | A_BOLD);
        refresh(); usleep(400000);
        clear(); refresh(); usleep(300000);
    }

    // Texto narrativo
    clear();
    attron(COLOR_PAIR(C_TEXT));
    mvprintw(mid - 6, 4, "Recuerdo la ultima vez que estuve en su casa.");
    mvprintw(mid - 4, 4, "Abri la puerta.");
    attroff(COLOR_PAIR(C_TEXT));
    refresh(); usleep(2200000);

    // El morado desaparece — aparece el rojo (el real, quieto)
    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(mid - 2, 4, "Lo que me perseguia...");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh(); usleep(1200000);

    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(mid - 1, 4, "...no era el.");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh(); usleep(1400000);

    // Novio real (rojo, quieto) + la otra persona (cafe)
    attron(COLOR_PAIR(C_REAL_HIM) | A_BOLD);
    mvprintw(mid + 1, 4, "EL");
    attroff(COLOR_PAIR(C_REAL_HIM) | A_BOLD);

    attron(COLOR_PAIR(C_ENEMY_OTH));
    mvprintw(mid + 1, 8, "OT");
    attroff(COLOR_PAIR(C_ENEMY_OTH));

    attron(COLOR_PAIR(C_TEXT));
    mvprintw(mid + 1, 12, "  <- estaban juntos. Quietos. Reales.");
    attroff(COLOR_PAIR(C_TEXT));
    refresh(); usleep(2500000);

    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(mid + 4, 4, "Era lo que yo construi en mi cabeza.");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh(); usleep(1800000);

    attron(COLOR_PAIR(C_TEXT));
    mvprintw(rows - 3, 4, "[ Presiona ENTER ]");
    attroff(COLOR_PAIR(C_TEXT));
    refresh();
}

// ── Carta final (revelación) ───────────────────────────────────────────
void drawEnding() {
    clear();
    int rows = getmaxy(stdscr);
    int mid  = rows / 2;

    // Fade in lento línea por línea
    const char* lines[] = {
        "Recuerdo la ultima carta que te escribi.",
        "",
        "Te decia que podiamos solucionar,",
        "que sabia que aun me amabas.",
        "",
        "Realmente nunca pense que llegaria a estos extremos.",
        "",
        "Nunca te culpes por lo que paso.",
        "Todos tenemos derecho a seguir con nuestras vidas.",
        nullptr
    };

    int startRow = mid - 5;
    for (int i = 0; lines[i] != nullptr; i++) {
        attron(COLOR_PAIR(C_TEXT));
        mvprintw(startRow + i, 6, "%s", lines[i]);
        attroff(COLOR_PAIR(C_TEXT));
        refresh();
        usleep(900000);
    }

    usleep(2000000);
    attron(COLOR_PAIR(C_TEXT) | A_DIM);
    mvprintw(rows - 3, 4, "[ Q para salir ]");
    attroff(COLOR_PAIR(C_TEXT) | A_DIM);
    refresh();
}

// ── Game Over ──────────────────────────────────────────────────────────
void drawGameOver() {
    clear();
    int rows = getmaxy(stdscr);
    int mid  = rows / 2;
    printCentered(mid - 1, "Te encontro.", C_RED, A_BOLD);
    printCentered(mid + 2,  "[ R para reiniciar | Q para salir ]", C_TEXT, 0);
    refresh();
}
