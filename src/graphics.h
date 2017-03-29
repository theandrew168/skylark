#ifndef SKYLARK_GRAPHICS_H
#define SKYLARK_GRAPHICS_H

#include <SDL2/SDL.h>
#include "types.h"

#define PIXEL_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

void graphics_init();
void graphics_terminate();
void graphics_clearScreen();
void graphics_clearPixels();
void graphics_draw();
void graphics_setPixelColor(uint8 r, uint8 g, uint8 b);
void graphics_setPixel(uint8 x, uint8 y, bool on);

#endif
