#include <stdio.h>
#include <unistd.h>
#include "chip8.h"
#include "input.h"

#define STEP 0

int main(int argc, char** argv) {

    /* Ensure a ROM was passed to skylark */
    if (argc != 2) {
        printf("Usage: %s <rom_file>\n", argv[0]);
        return 1;
    }

    /* Initialize emulator */
    if (!chip8_init()) {
        return 1;
    }

    /* Load ROM into emulator */
    if (!chip8_load_rom(argv[1])) {
        return 1;
    }

    /* Loop until game ends */
    while (chip8_running()) {
        input_update();
        chip8_emulate_cycle();

        /* Step the emulator with 'z' */
        while (STEP && !input_is_key_down(12)) {
            input_update();
            if (input_close_requested()) break;
        }

        usleep(1 * 1000);
    }

    chip8_terminate();

    return 0;
}
