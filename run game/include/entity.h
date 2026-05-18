#pragma once
#include "types.h"

void playerInit(Player* p, Vec2 startPos);
bool playerHasItem(const Player* p, int itemId);
void playerMove(Player* p, int dx, int dy, Tile map[MAP_H][MAP_W]);
void playerPickup(Player* p, Item items[MAX_ITEMS], int itemCount);

void enemyInit(Enemy* e, Vec2 pos, EnemyKind kind, int delay);
void enemiesUpdate(Enemy enemies[MAX_ENEMIES], int count,
                   const Player* player, Tile map[MAP_H][MAP_W]);
bool enemiesHitPlayer(const Enemy enemies[MAX_ENEMIES], int count,
                      const Player* player);
