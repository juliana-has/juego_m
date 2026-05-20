#include "../include/items.h"
#include <cstdlib>

// ============================================================
//  Textos de los items — el corazon narrativo del juego
// ============================================================

// Carta del novio — ROOM_2
// La clásica carta de un abusador: mezcla culpa, amor y excusas
static const char *ITEM_CONTENT_PAPER1 =
    "\n"
    "  Hola. Soy yo.\n"
    "\n"
    "  Se que estas enojada y tienes razon.\n"
    "  Pero necesito que entiendas que ese dia\n"
    "  no era yo. Tu lo sabes mejor que nadie.\n"
    "\n"
    "  Nadie me conoce como tu.\n"
    "  Nadie me aguanta como tu.\n"
    "  Eso no es cualquier cosa.\n"
    "\n"
    "  Juro que voy a cambiar.\n"
    "  Ya lo estoy haciendo.\n"
    "  Solo necesito que no te vayas.\n"
    "\n"
    "  Sin ti no soy nada.\n"
    "  Y tu sin mi tampoco.\n"
    "\n"
    "  Hablemos. Por favor.\n"
    "\n";

// Conversacion del celular — ROOM_4
// El papa lleva dias sin saber nada. Ella lo tranquiliza aunque no esta bien.
static const char *ITEM_CONTENT_PHONE =
    "\n"
    "  [14 llamadas perdidas — Papa]\n"
    "\n"
    "  Hace tres dias escribiste:\n"
    "\n"
    "  Tu: Todo bien, papa. No te preocupes.\n"
    "  Papa: Hija donde estas. Llamame.\n"
    "  Tu: Estoy con el. Ya vuelvo.\n"
    "  Papa: Cuando es ya.\n"
    "  Tu: Pronto.\n"
    "\n"
    "  Eso fue hace tres dias.\n"
    "\n"
    "  El ultimo mensaje de tu papa\n"
    "  dice solo:\n"
    "\n"
    "  \"Te quiero. Aqui estoy.\"\n"
    "\n";

// Carta final — ROOM_7
// La voz interna de Eli procesando por qué se quedó tanto tiempo
static const char *ITEM_CONTENT_PAPER2 =
    "\n"
    "  No se quien escribio esto.\n"
    "  Creo que fui yo.\n"
    "\n"
    "  El dia que decidi quedarme\n"
    "  no fue un dia de debilidad.\n"
    "  Fue un dia en que crei\n"
    "  que el amor podia mas\n"
    "  que el dolor.\n"
    "\n"
    "  Que si yo lo amaba suficiente\n"
    "  el iba a aprender a amarme bien.\n"
    "\n"
    "  Nadie me dijo que eso\n"
    "  no funciona asi.\n"
    "\n"
    "  Nadie te lo dice.\n"
    "\n";

// ============================================================
//  Nombres
// ============================================================
static const char *ITEM_NAME_PAPER1 = "Su carta";
static const char *ITEM_NAME_KEY    = "Llave";
static const char *ITEM_NAME_CANDLE = "Vela";
static const char *ITEM_NAME_PHONE  = "Celular";
static const char *ITEM_NAME_PAPER2 = "Carta tuya";

// ============================================================
//  items_init
// ============================================================
void items_init(Item *items, int *count) {
    *count = 5;

    items[0].id        = ITEM_PAPER1;
    items[0].pos.r     = 12;
    items[0].pos.c     = 5;
    items[0].collected = false;
    items[0].symbol    = '?';

    items[1].id        = ITEM_KEY;
    items[1].pos.r     = 13;
    items[1].pos.c     = 20;
    items[1].collected = false;
    items[1].symbol    = 'k';

    items[2].id        = ITEM_CANDLE;
    items[2].pos.r     = 11;
    items[2].pos.c     = 8;
    items[2].collected = false;
    items[2].symbol    = 'i';

    items[3].id        = ITEM_PHONE;
    items[3].pos.r     = 17;
    items[3].pos.c     = 14;
    items[3].collected = false;
    items[3].symbol    = 'p';

    items[4].id        = ITEM_PAPER2;
    items[4].pos.r     = 19;
    items[4].pos.c     = 20;
    items[4].collected = false;
    items[4].symbol    = '!';
}

// ============================================================
//  Helpers
// ============================================================
static int abs_val2(int x) { return (x < 0) ? -x : x; }

Item* items_find_nearby(Item *items, int count, int r, int c) {
    for (int i = 0; i < count; i++) {
        Item *it = &items[i];
        if (!it->collected &&
            abs_val2(it->pos.r - r) <= 1 &&
            abs_val2(it->pos.c - c) <= 1) {
            return it;
        }
    }
    return 0;
}

void items_collect(Item *item) {
    item->collected = true;
}

int items_count_collected(const Item *items, int count) {
    int n = 0;
    for (int i = 0; i < count; i++)
        if (items[i].collected) n++;
    return n;
}

const char* items_get_content(int itemId) {
    if (itemId == ITEM_PAPER1) return ITEM_CONTENT_PAPER1;
    if (itemId == ITEM_PHONE)  return ITEM_CONTENT_PHONE;
    if (itemId == ITEM_PAPER2) return ITEM_CONTENT_PAPER2;
    return 0;
}

const char* items_get_name(int itemId) {
    if (itemId == ITEM_PAPER1) return ITEM_NAME_PAPER1;
    if (itemId == ITEM_KEY)    return ITEM_NAME_KEY;
    if (itemId == ITEM_CANDLE) return ITEM_NAME_CANDLE;
    if (itemId == ITEM_PHONE)  return ITEM_NAME_PHONE;
    if (itemId == ITEM_PAPER2) return ITEM_NAME_PAPER2;
    return "?";
}
