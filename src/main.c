#include <stdio.h>
#include "graphics.h"
#include "input.h"
#include "chip8.h"

int main(int argc, char** argv) {
    int x;

    /* Ensure a ROM was passed to skylark */
    if (argc != 2) {
        printf("Usage: %s <rom_file>\n", argv[0]);
        return 1;
    }

    if (!graphics_init("Skylark Chip8 Emulator", 16)) {
        return 1;
    }

    graphics_set_pixel_color(128, 0, 128);
    input_init();

    chip8_init();
    if (!chip8_load_rom(argv[1])) {
        return 1;
    }

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
    }

    graphics_terminate();

    return 0;
}
