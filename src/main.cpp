#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <filesystem>
#include "chip8.h"

const int CHIP8_W = 64;
const int CHIP8_H = 32;
const int SCALE   = 15;

const int MENU_W = 640;
const int MENU_H = 480;

const std::string roms_folder = "roms";

// ---------------- Кнопка ----------------
struct Button {
    SDL_Rect rect;
    std::string text;
    bool hovered = false;
    bool pressed = false;
    std::function<void()> onClick;

    bool contains(int x, int y) const {
        return x >= rect.x && x < rect.x + rect.w &&
               y >= rect.y && y < rect.y + rect.h;
    }
};

// ---------------- Состояние приложения ----------------
enum class State { Menu, Game };

struct App {
    SDL_Window*   win    = nullptr;
    SDL_Renderer* ren    = nullptr;
    SDL_Texture*  screen = nullptr; // текстура 64x32 для Chip-8
    TTF_Font*     font   = nullptr;

    State state = State::Menu;

    std::vector<Button> menuButtons;

    bool running = true;

    // Список ROM'ов (лежат в корне проекта)
    std::vector<std::string> roms;
};

// ---------------- Инициализация ----------------
bool initSDL(App& app) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init: " << TTF_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    // Окно меню больше, чем игровое — сделаем его квадратным
    app.win = SDL_CreateWindow("Chip-8",
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                MENU_W, MENU_H, SDL_WINDOW_SHOWN);
    if (!app.win) return false;

    app.ren = SDL_CreateRenderer(app.win, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app.ren) return false;

    // Текстура 64x32 — всегда, независимо от состояния
    app.screen = SDL_CreateTexture(app.ren,
                    SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    CHIP8_W, CHIP8_H);
    if (!app.screen) return false;

    std::string rom = roms_folder; // копируем чтобы добавить слеш, потому что имя папки const
    rom.append("/");

    if (std::filesystem::exists(rom) && std::filesystem::is_directory(rom)) {
        for (auto& entry : std::filesystem::directory_iterator(rom)) {
            if (entry.path().extension() == ".ch8" || entry.path().extension() == ".c8"){
                app.roms.push_back(rom + entry.path().filename().string());
            }
        }
    } else {
        std::cerr << "Warning: folder 'roms' not found\n";
    }

    return true;
}

bool initFont(App& app) {
    const char* fontPath = "/usr/share/fonts/google-noto-vf/NotoSansMono[wght].ttf";
    app.font = TTF_OpenFont(fontPath, 20);
    if (!app.font) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << "\n";
        return false;
    }
    return true;
}

// ---------------- Текст ----------------
void drawText(SDL_Renderer* ren, TTF_Font* font,
              const std::string& text, int x, int y,
              SDL_Color color, bool centered = false, int boxW = 0)
{
    if (!font || text.empty()) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    if (!tex) { SDL_FreeSurface(surf); return; }

    SDL_Rect dst;
    dst.w = surf->w;
    dst.h = surf->h;
    dst.x = centered ? x + (boxW - surf->w) / 2 : x;
    dst.y = y;

    SDL_RenderCopy(ren, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

// ---------------- Меню ----------------
void startGame(App& app, chip8& c8, const std::string& rom) {
    if (!c8.loadApplication(rom.data())) {
        std::cerr << "Failed to load ROM: " << rom << "\n";
        return;
    }
    // c8.reset(); // если у chip8 есть метод reset — вызовите; иначе уберите
    
    app.state = State::Game;
    SDL_SetWindowSize(app.win, CHIP8_W * SCALE, CHIP8_H * SCALE);
}

void createMenu(App& app, chip8& c8) {
    app.menuButtons.clear();

    const int BTN_W = 260;
    const int BTN_H = 40;
    const int GAP   = 10;
    const int X     = 20;
    int y = 80;

    for (const auto& rom : app.roms) {
        Button b;
        b.rect = { X, y, BTN_W, BTN_H };
        b.text = rom.substr(roms_folder.size() + 1, (rom.size() + 1) - roms_folder.size());
        b.onClick = [&app, &c8, rom] { startGame(app, c8, rom); };
        app.menuButtons.push_back(b);
        y += BTN_H + GAP;
    }
}

// ---------------- Обработка событий ----------------
void updateKeys(chip8& c8, const SDL_Event& e, bool pressed) {
    switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_1: c8.key[0x1] = pressed; break;
        case SDL_SCANCODE_2: c8.key[0x2] = pressed; break;
        case SDL_SCANCODE_3: c8.key[0x3] = pressed; break;
        case SDL_SCANCODE_4: c8.key[0xC] = pressed; break;

        case SDL_SCANCODE_Q: c8.key[0x4] = pressed; break;
        case SDL_SCANCODE_W: c8.key[0x5] = pressed; break;
        case SDL_SCANCODE_E: c8.key[0x6] = pressed; break;
        case SDL_SCANCODE_R: c8.key[0xD] = pressed; break;

        case SDL_SCANCODE_A: c8.key[0x7] = pressed; break;
        case SDL_SCANCODE_S: c8.key[0x8] = pressed; break;
        case SDL_SCANCODE_D: c8.key[0x9] = pressed; break;
        case SDL_SCANCODE_F: c8.key[0xE] = pressed; break;

        case SDL_SCANCODE_Z: c8.key[0xA] = pressed; break;
        case SDL_SCANCODE_X: c8.key[0x0] = pressed; break;
        case SDL_SCANCODE_C: c8.key[0xB] = pressed; break;
        case SDL_SCANCODE_V: c8.key[0xF] = pressed; break;
    }
}

void handleMenuEvent(App& app, const SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        app.running = false;
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        for (auto& b : app.menuButtons)
            b.hovered = b.contains(e.motion.x, e.motion.y);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        for (auto& b : app.menuButtons)
            if (b.contains(e.button.x, e.button.y))
                b.pressed = true;
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        for (auto& b : app.menuButtons) {
            if (b.pressed && b.contains(e.button.x, e.button.y) && b.onClick)
                b.onClick();
            b.pressed = false;
        }
    }
}

void handleGameEvent(App& app, chip8& c8, const SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        app.running = false;
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            app.state = State::Menu;
            SDL_SetWindowSize(app.win, MENU_W, MENU_H);
            return;
        }
        updateKeys(c8, e, true);
    }

    if (e.type == SDL_KEYUP) {
        updateKeys(c8, e, false);
    }
}

// ---------------- Отрисовка меню ----------------
void drawButton(SDL_Renderer* ren, TTF_Font* font, const Button& b) {
    SDL_Color bg = b.pressed ? SDL_Color{ 70, 70, 70, 255 }
                : b.hovered ? SDL_Color{ 90, 90, 90, 255 }
                            : SDL_Color{ 50, 50, 50, 255 };
    SDL_SetRenderDrawColor(ren, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(ren, &b.rect);

    SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
    SDL_RenderDrawRect(ren, &b.rect);

    if (font) {
        // Центрируем текст и по вертикали, и по горизонтали
        int texH = TTF_FontHeight(font);
        drawText(ren, font, b.text,
                 b.rect.x,
                 b.rect.y + (b.rect.h - texH) / 2,
                 SDL_Color{ 255, 255, 255, 255 },
                 true, b.rect.w);
    }
}

void renderMenu(App& app) {
    SDL_SetRenderDrawColor(app.ren, 30, 30, 30, 255);
    SDL_RenderClear(app.ren);

    if (app.font) {
        drawText(app.ren, app.font, "CHIP-8",
                 20, 20,
                 SDL_Color{ 255, 255, 255, 255 },
                 true, MENU_W);
    }

    for (const auto& b : app.menuButtons)
        drawButton(app.ren, app.font, b);

    SDL_RenderPresent(app.ren);
}

// ---------------- Отрисовка игры ----------------
void renderGame(App& app, chip8& c8) {
    uint32_t* pixels;
    int pitch;
    SDL_LockTexture(app.screen, nullptr, (void**)&pixels, &pitch);

    for (int i = 0; i < CHIP8_W * CHIP8_H; i++) {
        pixels[i] = c8.gfx[i] ? 0xFFFFFFFF : 0xFF000000;
    }

    SDL_UnlockTexture(app.screen);

    SDL_RenderClear(app.ren);
    SDL_RenderCopy(app.ren, app.screen, nullptr, nullptr);
    SDL_RenderPresent(app.ren);
}

// ---------------- Главный цикл ----------------
void run(App& app, chip8& c8) {
    const int CYCLES_PER_FRAME = 5;
    Uint32 lastTimer = SDL_GetTicks();

    while (app.running) {
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
            // Эмуляция
            for (int i = 0; i < CYCLES_PER_FRAME; i++)
                c8.emulateCycle();

            // Таймеры 60 Гц
            Uint32 now = SDL_GetTicks();
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

    if (!initSDL(app) || !initFont(app)) {
        shutdown(app);
        return 1;
    }

    createMenu(app, c8);
    run(app, c8);
    shutdown(app);
    return 0;
}