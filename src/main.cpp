#include <iostream>
#include <SDL2/SDL.h>

#include "dm_chip-8.h"

#define SCREEN_WIDTH  64 * 12
#define SCREEN_HEIGHT 32 * 12

bool isRunning = true;

struct Settings {
    const char* filePath;
    bool debug = true;
    int stackSize = 12;
};

bool initSDL(SDL_Window** window, SDL_Renderer** renderer)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    *window = SDL_CreateWindow(
        "Chip-8 Emulator.",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    // error checking
    if (!window) 
    {
        std::cout << "Error: Could not create sdl window." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        return false;
    }
    if (!renderer)
    {
        std::cout << "Error: Could not create sdl renderer." << std::endl;
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Not working yet.
void getArgs(int argc, char* argv[], Settings& settings)
{
    for (size_t i = 0; i < argc; i++)
    {
        if (argv[i] == "-f")
        {
            settings.filePath = argv[i+1];
            std::cout << "file" << std::endl;
        }
        
    }
    
}

int main(int argc, char* argv[]){
    
    Settings settings;
    DMChip_8 chip_8;

    SDL_Window* window;
    SDL_Renderer* renderer;
    getArgs(argc, argv, settings);

    initSDL(&window, &renderer);

    chip_8.reset();
    if (!chip_8.loadRom("../test_roms/1-chip8-logo.ch8"))
    {
        std::cout << "Error: Failed to load rom." << std::endl;
    }
        
    chip_8.framebufferSetPixel(32, 16);
    
    while (isRunning)
    {
        chip_8.clock();

        // set background color to black and clear.
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        // Draw the frame buffer.
        for (size_t i = 0; i < 64; i++)
        {
            for (size_t j = 0; j < 32; j++)
            {
                if (chip_8.getFrameBuffer()[i * j]) {
                    SDL_Rect rect;
                    rect.w = SCREEN_WIDTH / 64;
                    rect.h = SCREEN_HEIGHT / 32,
                    rect.x = i * rect.w;
                    rect.y = j * rect.h;

                    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(4000); // TODO: set to the right speed.
    }
}
