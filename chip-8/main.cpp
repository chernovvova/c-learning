#include <iostream>

#include "Chip.h"
#include "SDL3/SDL.h"

constexpr int SCALE = 16;
constexpr int FRAME_MS = 1000 / 60;

void handle_keyboard_event(std::array<bool, 16> &keyboard, const SDL_Keycode keycode, const bool is_downed) {
    switch (keycode) {
        case SDLK_X: keyboard[0] = is_downed; break;
        case SDLK_1: keyboard[1] = is_downed; break;
        case SDLK_2: keyboard[2] = is_downed; break;
        case SDLK_3: keyboard[3] = is_downed; break;
        case SDLK_Q: keyboard[4] = is_downed; break;
        case SDLK_W: keyboard[5] = is_downed; break;
        case SDLK_E: keyboard[6] = is_downed; break;
        case SDLK_A: keyboard[7] = is_downed; break;
        case SDLK_S: keyboard[8] = is_downed; break;
        case SDLK_D: keyboard[9] = is_downed; break;
        case SDLK_Z: keyboard[10] = is_downed; break;
        case SDLK_C: keyboard[11] = is_downed; break;
        case SDLK_4: keyboard[12] = is_downed; break;
        case SDLK_R: keyboard[13] = is_downed; break;
        case SDLK_F: keyboard[14] = is_downed; break;
        case SDLK_V: keyboard[15] = is_downed; break;
        default: std::cerr << "Unknown keycode: " << keycode << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_GetError();
    }

    const std::filesystem::path rom_path = "resources/games/snake.ch8";
    Chip chip8{};
    chip8.readROM(rom_path);

    SDL_Window * window = SDL_CreateWindow("chip8", 64 * SCALE, 32 * SCALE, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, nullptr);

    bool running = true;

    while (running) {
        Uint64 frame_start = SDL_GetTicks();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                handle_keyboard_event(chip8.keyboard, event.key.key, true);
            }
            else if (event.type == SDL_EVENT_KEY_UP) {
                handle_keyboard_event(chip8.keyboard, event.key.key, false);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int x = 0; x < chip8.screen.size(); x++) {
            for (int y = 0; y < chip8.screen[0].size(); y++) {
                if (!chip8.screen[x][y]) {
                    continue;
                }

                SDL_FRect rect = {
                    static_cast<float>(x * SCALE),
                    static_cast<float>(y * SCALE),
                    static_cast<float>(SCALE),
                    static_cast<float>(SCALE)
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);

        for (int i = 0; i < 12; i++) {
            chip8.cycle();
        }
        chip8.tick_timers();

        Uint64 elapsed = SDL_GetTicks() - frame_start;   // сколько заняла работа
        if (elapsed < FRAME_MS) {
            SDL_Delay(FRAME_MS - elapsed);               // досыпаем остаток
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
