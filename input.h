#ifndef MAP_H
#define MAP_H

#include "types.h"

// ============================================================
//  Módulo de mapa
// ============================================================

// Arreglo estático del mapa (ROWS x COLS)
extern int gMap[ROWS][COLS];

// Inicializa el mapa con paredes, suelos, puertas y armarios
void map_init();

// Devuelve true si (r,c) es transitable (suelo, puerta o armario)
bool map_can_walk(int r, int c);

// Devuelve el tipo de tile en (r,c)
int  map_get_tile(int r, int c);

// Devuelve el ID de habitación según coordenadas
int  map_get_room(int r, int c);

// Dibuja el mapa en consola desde la posición del cursor (0,0)
// Recibe posición del jugador, enemigos e ítems para superponerlos
void map_draw(const struct DrawData *dd);

// Estructura auxiliar para dibujar todo en una pasada
struct EntityDraw {
    int  r;
    int  c;
    char symbol;
    bool active;
};

struct ItemDraw {
    int  r;
    int  c;
    char symbol;
    bool collected;
};

struct DrawData {
    EntityDraw entities[MAX_ENEMIES + 1]; // [0]=player, [1]=bf, [2]=conscience
    ItemDraw   items[MAX_ITEMS];
    int        itemCount;
    int        entityCount;
};

#endif // MAP_H
