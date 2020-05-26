#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <rom_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "failed to open rom: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t* buf = malloc(size);
    if (buf == NULL) {
        fclose(fp);
        fprintf(stderr, "failed to allocate buffer to hold ROM contents\n");
        return EXIT_FAILURE;
    }

    long count = fread(buf, 1, size, fp);
    if (count != size) {
        free(buf);
        fclose(fp);
        fprintf(stderr, "failed to read ROM into buffer\n");
        return EXIT_FAILURE;
    }
    fclose(fp);

    struct chip8 chip8 = { 0 };
    int rc = chip8_init(&chip8, buf, size);
    if (rc != CHIP8_OK) {
        free(buf);
        fprintf(stderr, "failed to init chip8 emulator\n");
        return EXIT_FAILURE;
    }
    free(buf);

    bool running = true;
    while (running) {
        // TODO input

        rc = chip8_step(&chip8);
        if (rc != CHIP8_OK) {
            running = false;
        }

        // TODO graphics
        for (long i = 0; i < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; i++) {
            printf("%s", chip8.display[i] ? "XX" : "  ");
            if (i % CHIP8_DISPLAY_WIDTH == CHIP8_DISPLAY_WIDTH - 1) printf("\n");
        }

        // TODO sound?
    }

    return EXIT_SUCCESS;
}
