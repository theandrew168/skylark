#include <stdio.h>
#include "graphics.h"
#include "input.h"
#include "chip8.h"

int main() {
    int x;

    graphics_init("Skylark Chip8 Emulator", 16);
    input_init();

    chip8_init();
    chip8_load_rom("roms/PONG");

    graphics_set_pixel_color(128, 0, 128);
    while (!input_close_requested()) {
        graphics_clear_pixels();
        graphics_clear_screen();
        input_update();

        for (x = 0; x < 16; x++) {
            graphics_set_pixel(x, x, true);
            if (input_is_key_down(x)) {
                printf("Key down: %u\n", x);
            }
        }

        graphics_draw();
        SDL_Delay(500);
    }

    graphics_terminate();

    return 0;
}
