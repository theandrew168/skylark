#include "graphics.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "types.h"

typedef struct screen_t {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8 pixel_color[3];
} screen_t;

static screen_t screen;

void graphics_init() {
    screen.window = NULL;
    screen.renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL2. SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    screen.window = SDL_CreateWindow("Skylark Chip8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * PIXEL_SIZE, SCREEN_HEIGHT * PIXEL_SIZE,
        SDL_WINDOW_SHOWN);
    if (!screen.window) {
        printf("Failed to create SDL2 window. SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    screen.renderer = SDL_CreateRenderer(screen.window, -1, SDL_RENDERER_ACCELERATED);
    if (!screen.renderer) {
        printf("Failed to create SDL2 renderer. SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(screen.window);
        exit(-1);
    }

    graphics_setPixelColor(255, 255, 255);
    graphics_clearPixels();
}

void graphics_terminate() {
    SDL_DestroyWindow(screen.window);
    SDL_Quit();
}

void graphics_clearScreen() {
    SDL_RenderClear(screen.renderer);
}

void graphics_clearPixels() {
    size_t i;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screen.pixels[i] = false;
    }
}

void graphics_draw() {
    size_t x, y;
    SDL_Rect pixel;

    pixel.w = PIXEL_SIZE;
    pixel.h = PIXEL_SIZE;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            pixel.x = x * PIXEL_SIZE;
            pixel.y = y * PIXEL_SIZE;

            if (screen.pixels[x + y * SCREEN_WIDTH]) {
                SDL_SetRenderDrawColor(screen.renderer,
                    screen.pixel_color[0],
                    screen.pixel_color[1],
                    screen.pixel_color[2],
                    255);
            } else {
                SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255);
            }
                
            SDL_RenderFillRect(screen.renderer, &pixel);
        }
    }

    SDL_RenderPresent(screen.renderer);
}

void graphics_setPixelColor(uint8 r, uint8 g, uint8 b) {
    screen.pixel_color[0] = r;
    screen.pixel_color[1] = g;
    screen.pixel_color[2] = b;
}

void graphics_setPixel(uint8 x, uint8 y, bool on) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        printf("Attempt to set invalid pixel: (%u, %u)\n", x, y);
        return;
    }

    screen.pixels[x + y * SCREEN_WIDTH] = on;
}
