#include "graphics.h"
#include "chip8.h"

int main() {
    int x, y;

    graphics_init();

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            if (x == y) continue;
            graphics_setPixel(x, y, true);
            graphics_draw();
        }
    }

    SDL_Delay(2000);

    graphics_clearScreen();
    graphics_clearPixels();
    graphics_draw();

    SDL_Delay(1000);

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            if (x != y) continue;
            graphics_setPixel(x, y, true);
        }
    }

    graphics_setPixelColor(128, 0, 128);
    graphics_draw();
    SDL_Delay(2000);
    graphics_terminate();

    return 0;
}
