#include <stdio.h>
#include "graphics.h"
#include "input.h"
#include "chip8.h"

int main(int argc, char** argv) {
    int i;

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

        chip8_emulate_cycle();

        input_update();
        for (i = 0; i < NUM_KEYS; i++) {
            chip8_set_key(i, input_is_key_down(i));
        }

        if (chip8_draw_requested()) {
            graphics_draw();
        }
    }

    graphics_terminate();

    return 0;
}
