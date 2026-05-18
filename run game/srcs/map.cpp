#include "map.h"
#include "entity.h"
#include <string.h>

Tile* mapTileAt(Tile map[MAP_H][MAP_W], int x, int y) { return &map[y][x]; }

bool isWalkable(Tile map[MAP_H][MAP_W], int x, int y) {
    if (x<0||x>=MAP_W||y<0||y>=MAP_H) return false;
    Tile t = map[y][x];
    return t==TILE_FLOOR||t==TILE_DOOR||t==TILE_EXIT||t==TILE_PATH;
}

static void fillRect(Tile map[MAP_H][MAP_W], int x, int y, int w, int h, Tile t) {
    for (int r=y; r<y+h&&r<MAP_H; r++)
        for (int c=x; c<x+w&&c<MAP_W; c++) map[r][c]=t;
}

static void drawRoom(Tile map[MAP_H][MAP_W], int x, int y, int w, int h) {
    for (int c=x; c<x+w; c++) { map[y][c]=TILE_WALL; map[y+h-1][c]=TILE_WALL; }
    for (int r=y; r<y+h; r++) { map[r][x]=TILE_WALL; map[r][x+w-1]=TILE_WALL; }
    fillRect(map, x+1, y+1, w-2, h-2, TILE_FLOOR);
}

// ── MAPA 1: Casa del novio ─────────────────────────────────────────────
void buildHouseMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                   Item items[MAX_ITEMS], int* itemCount,
                   Letter letters[MAX_LETTERS], int* letterCount,
                   Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount) {

    memset(map, TILE_EMPTY, sizeof(Tile)*MAP_H*MAP_W);
    *roomCount=0; *itemCount=0; *letterCount=0; *enemyCount=0;

    // Habitación 0: Dormitorio
    drawRoom(map, 1, 1, 12, 8);
    rooms[0] = Room(1,1,12,8,true,Vec2{12,4});

    // Habitación 1: Pasillo
    drawRoom(map, 13, 1, 5, 18);
    rooms[1] = Room(13,1,5,18,true,Vec2{18,9});
    map[4][12]=TILE_DOOR; map[4][13]=TILE_FLOOR;

    // Habitación 2: Sala
    drawRoom(map, 1, 10, 12, 9);
    rooms[2] = Room(1,10,12,9,true,Vec2{12,14});
    map[14][12]=TILE_DOOR; map[14][13]=TILE_FLOOR;

    // Habitación 3: Cocina
    drawRoom(map, 19, 1, 10, 8);
    rooms[3] = Room(19,1,10,8,true,Vec2{19,4});
    map[4][18]=TILE_FLOOR; map[4][19]=TILE_DOOR;

    // Habitación 4: Baño
    drawRoom(map, 19, 10, 10, 9);
    rooms[4] = Room(19,10,10,9,true,Vec2{19,14});
    map[14][18]=TILE_FLOOR; map[14][19]=TILE_DOOR;

    // Habitación 5: Garaje / Salida al bosque
    drawRoom(map, 30, 4, 9, 12);
    rooms[5] = Room(30,4,9,12,true,Vec2{38,9});
    map[9][29]=TILE_FLOOR; map[9][30]=TILE_DOOR;
    map[9][18]=TILE_FLOOR;
    map[9][38]=TILE_EXIT;

    *roomCount = 6;
    playerStart->x = 6; playerStart->y = 4;

    // Ítems coleccionables en la casa del novio
    items[0] = {{3,  3},  true, ITEM_COLLAR, "Collar", "Tu collar. Lo dejaste aqui."};
    items[1] = {{8,  14}, true, ITEM_CARTA,  "Carta",  "Una carta a medias. Nunca la terminaste."};
    items[2] = {{32, 7},  true, ITEM_VELA,   "Vela",   "Una vela apagada. Huele a el."};
    *itemCount = 3;

    // Fragmentos narrativos
    letters[0] = {{3,  6},  true, "Mensaje de el: 'donde estas? ya llego'", false};
    letters[1] = {{8,  12}, true, "Hay un vaso roto en el suelo.", false};
    *letterCount = 2;

    // Enemigos: EL (morado, lento) + 2 amigos (amarillos, rapidos)
    enemyInit(&enemies[0], {5,12}, ENEMY_HIM,    7);
    enemyInit(&enemies[1], {22,4}, ENEMY_FRIEND, 3);
    enemyInit(&enemies[2], {24,14},ENEMY_FRIEND, 4);
    *enemyCount = 3;
}

// ── MAPA 2: Bosque ─────────────────────────────────────────────────────
void buildForestMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                    Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount) {

    memset(map, TILE_EMPTY, sizeof(Tile)*MAP_H*MAP_W);
    *roomCount=0; *enemyCount=0;

    for (int r=0; r<MAP_H; r++)
        for (int c=0; c<MAP_W; c++) map[r][c]=TILE_TREE;

    int pathY = 10;
    for (int c=0; c<MAP_W; c++) {
        if (c%8==0 && pathY>3)  pathY--;
        if (c%7==0 && pathY<16) pathY++;
        map[pathY][c] = TILE_PATH;
        if (pathY>0)       map[pathY-1][c] = TILE_PATH;
        if (pathY<MAP_H-1) map[pathY+1][c] = TILE_PATH;
    }
    map[10][0]       = TILE_EXIT;
    map[10][MAP_W-1] = TILE_EXIT;

    rooms[0] = Room(0,0,MAP_W,MAP_H,true,Vec2{MAP_W-1, 10});
    *roomCount = 1;

    playerStart->x = 1; playerStart->y = 10;

    enemyInit(&enemies[0], {5,10},  ENEMY_HIM,    8);
    enemyInit(&enemies[1], {35,10}, ENEMY_FRIEND, 4);
    *enemyCount = 2;
}

// ── MAPA 3: Casa de ella ───────────────────────────────────────────────
void buildHomeMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                  Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount) {

    memset(map, TILE_EMPTY, sizeof(Tile)*MAP_H*MAP_W);
    *roomCount=0; *enemyCount=0;

    // Recibidor
    drawRoom(map, 5, 14, 12, 5);
    rooms[0] = Room(5,14,12,5,true,Vec2{11,14});

    // Sala
    drawRoom(map, 5, 8, 12, 6);
    rooms[1] = Room(5,8,12,6,true,Vec2{11,8});
    map[13][11]=TILE_DOOR;

    // Cuarto
    drawRoom(map, 18, 8, 14, 11);
    rooms[2] = Room(18,8,14,11,true,Vec2{18,13});
    map[13][17]=TILE_FLOOR; map[13][18]=TILE_DOOR;

    // Cocina
    drawRoom(map, 5, 1, 12, 7);
    rooms[3] = Room(5,1,12,7,true,Vec2{11,7});
    map[8][11]=TILE_DOOR;

    // Baño
    drawRoom(map, 18, 1, 10, 7);
    rooms[4] = Room(18,1,10,7,true,Vec2{18,4});
    map[4][17]=TILE_FLOOR; map[4][18]=TILE_DOOR;

    // Terraza
    drawRoom(map, 33, 8, 6, 11);
    rooms[5] = Room(33,8,6,11,true,Vec2{33,13});
    map[13][32]=TILE_FLOOR; map[13][33]=TILE_DOOR;

    *roomCount = 6;

    // Entrada desde el bosque
    map[18][11] = TILE_EXIT;

    playerStart->x = 11; playerStart->y = 17;

    // En la fase HOME no hay enemigos al inicio —
    // el morado aparece tras la nota (lo activa main.cpp)
    *enemyCount = 0;
    for (int i=0; i<MAX_ENEMIES; i++) enemies[i].active = false;
}
