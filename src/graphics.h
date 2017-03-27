#ifndef SKYLARK_GRAPHICS_H
#define SKYLARK_GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define PIXEL_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct screen_t {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t pixel_color[3];
} screen_t;

void graphics_init();
void graphics_terminate();
void graphics_clearScreen();
void graphics_clearPixels();
void graphics_draw();
void graphics_setPixelColor(uint8_t r, uint8_t g, uint8_t b);
void graphics_setPixel(uint8_t x, uint8_t y, bool on);

#endif
