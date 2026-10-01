
#ifdef _WIN32
    #define SDL_MAIN_HANDLED
    #include <SDL.h>
    #include <SDL_ttf.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
#endif
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <filesystem>
#include "gui.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

const int CYCLES_PER_FRAME = 5;

// ---------------- Главный цикл ----------------
void main_loop(App& app, chip8& c8){
    static Uint32 lastTimer = SDL_GetTicks();
    SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (app.state == State::Menu)
                handleMenuEvent(app, e);
            else
                handleGameEvent(app, c8, e);
        }

        if (app.state == State::Menu) {
            renderMenu(app);
        } else {
            for (int i = 0; i < CYCLES_PER_FRAME; i++)
                c8.run(); 

            uint32_t now = SDL_GetTicks();
            if (now - lastTimer >= 16) {
                c8.updateTimers();
                lastTimer = now;
            }

            if (c8.drawFlag) {
                renderGame(app, c8);
                c8.drawFlag = false;
            }
        }
}

void run(App& app, chip8& c8) {
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop([]() {main_loop(app, c8)}, 0, 1);
    #else
        while (app.running) { main_loop(app, c8); }
    #endif
}



// ---------------- Завершение ----------------
void shutdown(App& app) {
    if (app.font)   TTF_CloseFont(app.font);
    if (app.screen) SDL_DestroyTexture(app.screen);
    if (app.ren)    SDL_DestroyRenderer(app.ren);
    if (app.win)    SDL_DestroyWindow(app.win);
    TTF_Quit();
    SDL_Quit();
}

// ---------------- main ----------------
int main(int argc, char* argv[]) {
    chip8 c8;
    App app;

    if (!initSDL(app)) {
        shutdown(app);
        return 1;
    }

    initFont(app);

    createMenu(app, c8);
    run(app, c8);
    shutdown(app);
    return 0;
}