#include "chip8.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Chip8 font information
 */
uint8_t chip8_fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

chip8_t chip8;

void chip8_init() {
    memset(chip8.memory, 0, 4096);
    memset(chip8.V, 0, 16);
    
    chip8.I = 0;
    chip8.pc = 0x200;

    memset(chip8.stack, 0, 16);
    chip8.sp = 0;

    memset(chip8.keys, 0, 16);

    memset(chip8.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
    chip8.timer_delay = 0;
    chip8.timer_sound = 0;

    chip8.drawRequested = false;
}

void chip8_terminate() {
    /* Nothing to do here? */
}

void chip8_loadROM(const char* rom) {
    size_t i, length, bytes_read;
    uint8_t* buffer;

    FILE* file = fopen(rom, "rb");
    if (!file) {
        printf("Unable to locate ROM: %s\n", rom);
        return;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);

    buffer = (uint8_t*)malloc(sizeof(uint8_t) * length);
    if (!buffer) {
        printf("Failed to allocate memory for ROM: %s\n", rom);
        fclose(file);
        return;
    }

    bytes_read = fread(buffer, 1, length, file);
    if (bytes_read != length) {
        printf("Failed to read ROM: %s\n", rom);
        free(buffer);
        fclose(file);
    }

    if (length < (0xFFF - 0x200)) {
        for (i = 0; i < length; i++) {
            chip8.memory[0x200 + i] = buffer[i];
        }
    } else {
        printf("ROM too big for memory: %s\n", rom);
    }

    fclose(file);
    free(buffer);
}

void chip8_emulateCycle() {

}

void chip8_updateInput() {

}

bool chip8_drawRequested() {
    if (chip8.drawRequested) {
        chip8.drawRequested = false;
        return true;
    } else {
        return false;
    }
}
