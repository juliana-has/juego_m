#ifndef ITEMS_H
#define ITEMS_H

#include "types.h"

// ============================================================
//  Modulo de items coleccionables
// ============================================================

struct Item {
    int  id;          // ITEM_*
    Vec2 pos;
    bool collected;
    char symbol;      // caracter para dibujar en mapa
};

// Inicializa todos los items en sus posiciones del mapa
void items_init(Item *items, int *count);

// Busca si hay un item en o adyacente a (r,c) que no haya sido recogido.
// Retorna puntero al item, o NULL si no hay ninguno.
Item* items_find_nearby(Item *items, int count, int r, int c);

// Marca el item como recogido
void items_collect(Item *item);

// Devuelve cuantos items han sido recogidos
int  items_count_collected(const Item *items, int count);

// Devuelve el texto/contenido de un item al "leerlo"
const char* items_get_content(int itemId);

// Devuelve el nombre visible del item
const char* items_get_name(int itemId);

#endif // ITEMS_H
