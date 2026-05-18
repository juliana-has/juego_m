#pragma once
#include "types.h"

void rendererInit();
void rendererShutdown();
void clearScreen();

void drawPoem(int step);           // 0, 1, 2 — pantallas de apertura
void drawIntro();                  // ¡CORRE!
void drawMap(const Tile map[MAP_H][MAP_W],
             const Player* player,
             const Enemy   enemies[MAX_ENEMIES],
             const Item    items[MAX_ITEMS],
             const Letter  letters[MAX_LETTERS],
             Phase phase);
void drawHUD(const Player* player, Phase phase);
void drawMessage(const char* msg);
void drawLetter(const char* text);
void drawHomeNote();               // papel + aparición del morado
void drawFlashback();              // el con alguien más (rojo)
void drawEnding();                 // carta final, revelación
void drawGameOver();
