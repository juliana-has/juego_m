#include "../include/enemy.h"
#include "../include/map.h"
#include <cstdlib>

// ============================================================
//  Helpers
// ============================================================
static int abs_val(int x) { return (x < 0) ? -x : x; }

static int sign(int x) {
    if (x > 0) return  1;
    if (x < 0) return -1;
    return 0;
}

static Vec2 step_toward(Vec2 from, Vec2 to) {
    int dr = to.r - from.r;
    int dc = to.c - from.c;
    Vec2 next = from;

    if (abs_val(dr) >= abs_val(dc)) {
        int nr = from.r + sign(dr);
        if (map_can_walk(nr, from.c)) { next.r = nr; return next; }
        int nc = from.c + sign(dc);
        if (map_can_walk(from.r, nc)) { next.c = nc; return next; }
    } else {
        int nc = from.c + sign(dc);
        if (map_can_walk(from.r, nc)) { next.c = nc; return next; }
        int nr = from.r + sign(dr);
        if (map_can_walk(nr, from.c)) { next.r = nr; return next; }
    }
    return from;
}

int enemy_dist(Vec2 a, Vec2 b) {
    return abs_val(a.r - b.r) + abs_val(a.c - b.c);
}

// ============================================================
//  enemy_init_all
// ============================================================
void enemy_init_all(Enemy *enemies, int maxCount) {

    // ---- Novio: rapido, patrulla agresiva, persecucion cercana ----
    Enemy *bf       = &enemies[0];
    bf->type        = ENT_BOYFRIEND;
    bf->active      = true;
    bf->chasing     = false;
    bf->tickCounter = 0;
    bf->patrolIdx   = 0;
    bf->pos.r       = 3;
    bf->pos.c       = 5;
    bf->lastKnownPlayer.r = 3;
    bf->lastKnownPlayer.c = 13;

    // Ruta amplia: recorre todas las habitaciones sin parar
    Vec2 bfRoute[MAX_PATROL] = {
        {3,5},  {3,10}, {6,10}, {6,5},
        {9,5},  {12,5}, {14,5}, {14,10},
        {16,5}, {19,5}, {20,8}, {19,3},
        {19,20},{14,20},{13,20},{9,20}
    };
    bf->patrolLen = MAX_PATROL;
    for (int i = 0; i < MAX_PATROL; i++) bf->patrol[i] = bfRoute[i];

    // ---- Conciencia: lenta, rango infinito, nunca patrulla ----
    // No tiene ruta — siempre va hacia donde esta Eli.
    // Su velocidad es menor pero jamas se detiene ni se distrae.
    Enemy *con       = &enemies[1];
    con->type        = ENT_CONSCIENCE;
    con->active      = false;
    con->chasing     = false;  // siempre "chasing" cuando activa, pero el flag no se usa igual
    con->tickCounter = 0;
    con->patrolIdx   = 0;
    con->pos.r       = 19;
    con->pos.c       = 16;
    con->lastKnownPlayer.r = 3;
    con->lastKnownPlayer.c = 13;

    // Ruta vacia — la conciencia no patrulla
    for (int i = 0; i < MAX_PATROL; i++) {
        con->patrol[i].r = 19;
        con->patrol[i].c = 16;
    }
    con->patrolLen = 1;

    for (int i = 2; i < maxCount; i++) {
        enemies[i].active = false;
        enemies[i].type   = -1;
    }
}

// ============================================================
//  enemy_update
// ============================================================
void enemy_update(Enemy *e, Vec2 playerPos) {
    if (!e->active) return;

    e->tickCounter++;

    if (e->type == ENT_BOYFRIEND) {
        // ---- Novio: rapido, persigue en rango corto, patrulla el resto ----
        // Velocidad base alta (ENEMY_SPEED ticks entre pasos)
        if (e->tickCounter < ENEMY_SPEED) return;
        e->tickCounter = 0;

        int d = enemy_dist(e->pos, playerPos);

        if (d <= CHASE_RANGE) {
            // Persecucion activa — va directo
            e->chasing = true;
            e->lastKnownPlayer = playerPos;
            e->pos = step_toward(e->pos, playerPos);
        } else if (e->chasing) {
            // Perdio de vista al jugador — va a la ultima posicion conocida
            int dl = enemy_dist(e->pos, e->lastKnownPlayer);
            if (dl <= 1) {
                // Llego al ultimo lugar conocido, vuelve a patrullar
                e->chasing = false;
            } else {
                e->pos = step_toward(e->pos, e->lastKnownPlayer);
            }
        } else {
            // Patrulla normal
            Vec2 wp = e->patrol[e->patrolIdx % e->patrolLen];
            if (enemy_dist(e->pos, wp) < 2)
                e->patrolIdx = (e->patrolIdx + 1) % e->patrolLen;
            e->pos = step_toward(e->pos, wp);
        }

    } else if (e->type == ENT_CONSCIENCE) {
        // ---- Conciencia: lenta, rango INFINITO, nunca para ----
        // Se mueve cada CONSCIENCE_SPEED * 2 ticks (mas lenta que el novio)
        // pero siempre sabe donde estas — no hay forma de despistarla,
        // solo de llegar al armario antes de que te alcance.
        int conSpeed = CONSCIENCE_SPEED * 2;
        if (e->tickCounter < conSpeed) return;
        e->tickCounter = 0;

        // Siempre va hacia el jugador — sin patrulla, sin rango minimo
        e->chasing = true;
        e->lastKnownPlayer = playerPos;
        e->pos = step_toward(e->pos, playerPos);
    }
}

// ============================================================
//  enemy_catches
// ============================================================
bool enemy_catches(const Enemy *e, Vec2 playerPos) {
    if (!e->active) return false;
    return (enemy_dist(e->pos, playerPos) <= 1);
}

// ============================================================
//  enemy_activate_conscience
// ============================================================
void enemy_activate_conscience(Enemy *enemies, int maxCount) {
    for (int i = 0; i < maxCount; i++) {
        if (enemies[i].type == ENT_CONSCIENCE) {
            enemies[i].active = true;
            // Aparece en la posicion actual del jugador conocida
            return;
        }
    }
}

// ============================================================
//  enemy_who_caught
// ============================================================
int enemy_who_caught(const Enemy *enemies, int count, Vec2 playerPos) {
    for (int i = 0; i < count; i++) {
        if (enemies[i].active && enemy_dist(enemies[i].pos, playerPos) <= 1)
            return enemies[i].type;
    }
    return -1;
}
