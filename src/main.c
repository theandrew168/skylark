#include "graphics.h"
#include "input.h"
#include "chip8.h"

int main() {
    int x;

    graphics_init();
    input_init();

    chip8_init();
    chip8_loadROM("roms/PONG");
    chip8_terminate();

    graphics_setPixelColor(128, 0, 128);
    while (!input_closeRequested()) {
        graphics_clearPixels();
        graphics_clearScreen();
        input_update();

        for (x = 0; x < 16; x++) {
            graphics_setPixel(x, x, true);
            if (input_isKeyDown(x)) {
                printf("Key down: %u\n", x);
            }
        }

        graphics_draw();
        SDL_Delay(500);
    }

    graphics_terminate();

    return 0;
}
