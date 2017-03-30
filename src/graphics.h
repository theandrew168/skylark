#ifndef SKYLARK_GRAPHICS_H
#define SKYLARK_GRAPHICS_H

#include <SDL2/SDL.h>
#include "types.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

void graphics_init(const char* title, int pixel_size);
void graphics_terminate();
void graphics_clear_screen();
void graphics_clear_pixels();
void graphics_draw();
void graphics_set_pixel_color(unsigned char r, unsigned char g, unsigned char b);
void graphics_set_pixel(int x, int y, bool on);

#endif
