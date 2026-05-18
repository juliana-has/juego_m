#include "entity.h"
#include "map.h"
#include <stdlib.h>

// ── Jugadora ───────────────────────────────────────────────────────────
void playerInit(Player* p, Vec2 startPos) {
    p->pos       = startPos;
    p->itemCount = 0;
    p->alive     = true;
    p->frozen    = false;
    for (int i = 0; i < INVENTORY_SIZE; i++) p->inventory[i] = -1;
}

bool playerHasItem(const Player* p, int itemId) {
    for (int i = 0; i < p->itemCount; i++)
        if (p->inventory[i] == itemId) return true;
    return false;
}

void playerMove(Player* p, int dx, int dy, Tile map[MAP_H][MAP_W]) {
    if (p->frozen) return;
    int nx = p->pos.x + dx;
    int ny = p->pos.y + dy;
    if (isWalkable(map, nx, ny)) {
        p->pos.x = nx;
        p->pos.y = ny;
    }
}

void playerPickup(Player* p, Item items[MAX_ITEMS], int itemCount) {
    if (p->itemCount >= INVENTORY_SIZE) return;
    for (int i = 0; i < itemCount; i++) {
        Item* it = &items[i];
        if (!it->onFloor) continue;
        if (it->pos.x == p->pos.x && it->pos.y == p->pos.y) {
            p->inventory[p->itemCount++] = it->id;
            it->onFloor = false;
            return;
        }
    }
}

// ── Enemigos ───────────────────────────────────────────────────────────
void enemyInit(Enemy* e, Vec2 pos, EnemyKind kind, int delay) {
    e->pos       = pos;
    e->kind      = kind;
    e->active    = true;
    e->moveTimer = 0;
    e->moveDelay = delay;
}

static void moveToward(Enemy* e, const Player* p, Tile map[MAP_H][MAP_W]) {
    int dx = 0, dy = 0;
    if      (p->pos.x > e->pos.x) dx =  1;
    else if (p->pos.x < e->pos.x) dx = -1;
    if      (p->pos.y > e->pos.y) dy =  1;
    else if (p->pos.y < e->pos.y) dy = -1;

    int ax = dx < 0 ? -dx : dx;
    int ay = dy < 0 ? -dy : dy;

    if (ax >= ay) {
        if (dx && isWalkable(map, e->pos.x+dx, e->pos.y)) { e->pos.x+=dx; return; }
        if (dy && isWalkable(map, e->pos.x, e->pos.y+dy)) { e->pos.y+=dy; return; }
    } else {
        if (dy && isWalkable(map, e->pos.x, e->pos.y+dy)) { e->pos.y+=dy; return; }
        if (dx && isWalkable(map, e->pos.x+dx, e->pos.y)) { e->pos.x+=dx; return; }
    }
}

static void moveErratic(Enemy* e, const Player* p, Tile map[MAP_H][MAP_W]) {
    if (rand() % 10 < 7) {
        moveToward(e, p, map);
    } else {
        int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        int d = rand() % 4;
        int nx = e->pos.x + dirs[d][0];
        int ny = e->pos.y + dirs[d][1];
        if (isWalkable(map, nx, ny)) { e->pos.x=nx; e->pos.y=ny; }
    }
}

void enemiesUpdate(Enemy enemies[MAX_ENEMIES], int count,
                   const Player* player, Tile map[MAP_H][MAP_W]) {
    for (int i = 0; i < count; i++) {
        Enemy* e = &enemies[i];
        if (!e->active) continue;
        if (++e->moveTimer < e->moveDelay) continue;
        e->moveTimer = 0;
        if (e->kind == ENEMY_HIM)
            moveToward(e, player, map);
        else
            moveErratic(e, player, map);
    }
}

bool enemiesHitPlayer(const Enemy enemies[MAX_ENEMIES], int count,
                      const Player* player) {
    for (int i = 0; i < count; i++) {
        if (!enemies[i].active) continue;
        if (enemies[i].pos.x == player->pos.x &&
            enemies[i].pos.y == player->pos.y) return true;
    }
    return false;
}
