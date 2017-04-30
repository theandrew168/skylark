#include "graphics.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "types.h"

typedef struct screen_t {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int pixel_size;
    bool pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    unsigned char pixel_r, pixel_g, pixel_b;
} screen_t;

static screen_t screen;

bool graphics_init(const char* title, int pixel_size) {
    screen.window = NULL;
    screen.renderer = NULL;
    screen.pixel_size = pixel_size;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL2. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    screen.window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * screen.pixel_size, SCREEN_HEIGHT * screen.pixel_size,
        SDL_WINDOW_SHOWN);
    if (!screen.window) {
        printf("Failed to create SDL2 window. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    screen.renderer = SDL_CreateRenderer(screen.window, -1, SDL_RENDERER_ACCELERATED);
    if (!screen.renderer) {
        printf("Failed to create SDL2 renderer. SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(screen.window);
        return false;
    }

    graphics_set_pixel_color(255, 255, 255);
    graphics_clear_pixels();

    return true;
}

void graphics_terminate() {
    SDL_DestroyWindow(screen.window);
    SDL_Quit();
}

void graphics_clear_screen() {
    SDL_RenderClear(screen.renderer);
}

void graphics_clear_pixels() {
    int i;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screen.pixels[i] = false;
    }
}

void graphics_draw() {
    int x, y;
    SDL_Rect pixel;

    pixel.w = screen.pixel_size;
    pixel.h = screen.pixel_size;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            pixel.x = x * screen.pixel_size;
            pixel.y = y * screen.pixel_size;

            if (screen.pixels[x + y * SCREEN_WIDTH]) {
                SDL_SetRenderDrawColor(screen.renderer,
                    screen.pixel_r, screen.pixel_g, screen.pixel_b, 255);
            } else {
                SDL_SetRenderDrawColor(screen.renderer, 0, 0, 0, 255);
            }
                
            SDL_RenderFillRect(screen.renderer, &pixel);
        }
    }

    SDL_RenderPresent(screen.renderer);
}

void graphics_set_pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    screen.pixel_r = r;
    screen.pixel_g = g;
    screen.pixel_b = b;
}

bool graphics_set_pixel(int x, int y, bool on) {
    bool collision = false;

    /* Handle wrap-arounds */
    if (x >= SCREEN_WIDTH) x = x - SCREEN_WIDTH;
    if (x < 0) x = x + SCREEN_WIDTH;
    if (y >= SCREEN_HEIGHT) y = y - SCREEN_HEIGHT;
    if (y < 0) y = y + SCREEN_HEIGHT;

    /* Check if pixel is being erased */
    if (screen.pixels[x + y * SCREEN_WIDTH] && on) {
        collision = true;
    }
    
    /* Manually XOR the pixels*/
    if ((screen.pixels[x + y * SCREEN_WIDTH] && !on) ||
        (!screen.pixels[x + y * SCREEN_WIDTH] && on)) {
        screen.pixels[x + y * SCREEN_WIDTH] = true;
    } else {
        screen.pixels[x + y * SCREEN_WIDTH] = false;
    }

    return collision;
}
