#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
static void sleep_ms(int ms) { Sleep(ms); }
#else
#include <unistd.h>
static void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

#include "include/types.h"
#include "include/game.h"
#include "include/input.h"
#include "include/renderer.h"
#include "include/audio.h"

int main() {
    input_init();
    audio_init();   // no-op si miniaudio.h no esta presente

    GameState gs;
    game_init(&gs);

    while (true) {
        if (gs.phase == PHASE_INTRO1 ||
            gs.phase == PHASE_INTRO2 ||
            gs.phase == PHASE_RUN    ||
            gs.phase == PHASE_DEAD   ||
            gs.phase == PHASE_WIN)
        {
            game_handle_intro(&gs);  // killedBy se pasa via render_screen desde game_handle_intro
            continue;
        }

        if (gs.phase == PHASE_PLAY) {
            game_tick(&gs);

            if (gs.phase == PHASE_PLAY) {
                render_home();
                render_map(
                    &gs.player,
                    gs.enemies, gs.enemyCount,
                    gs.items,   gs.itemCount);
                render_hud(
                    &gs.player,
                    gs.items,   gs.itemCount,
                    gs.conscienceActive,
                    gs.phase,
                    gs.elapsedSeconds,
                    gs.bestScore,
                    gs.hudMessage,
                    gs.hudMessageTicks);
            }

            sleep_ms(60);
        }
    }

    audio_shutdown();
    input_restore();
    return 0;
}
