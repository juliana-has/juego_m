#pragma once
#include "types.h"

Tile* mapTileAt(Tile map[MAP_H][MAP_W], int x, int y);
bool  isWalkable(Tile map[MAP_H][MAP_W], int x, int y);

void buildHouseMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                   Item items[MAX_ITEMS],  int* itemCount,
                   Letter letters[MAX_LETTERS], int* letterCount,
                   Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount);

void buildForestMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                    Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount);

void buildHomeMap(Tile map[MAP_H][MAP_W], Room rooms[MAX_ROOMS], int* roomCount,
                  Vec2* playerStart, Enemy enemies[MAX_ENEMIES], int* enemyCount);
