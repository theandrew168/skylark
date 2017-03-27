#ifndef SKYLARK_CHIP8_H
#define SKYLARK_CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "graphics.h"

/*
 * 2-byte opcodes
 */
typedef uint16_t opcode_t;

/*
 * 4K memory
 * 16 8-bit registers
 * Index register
 * Program counter
 *
 * Program stack (size 16)
 * Stack pointer
 *
 * 16-key input status
 *
 * 2048 pixels
 * Delay timer
 * Sound timer
 */
typedef struct chip8_t {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;

    uint16_t stack[16];
    uint16_t sp;

    uint8_t keys[16];

    uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t timer_delay;
    uint8_t timer_sound;
} chip8_t;

void chip8_init();
void chip8_terminate();
void chip8_loadROM(const char* rom);
void chip8_emulateCycle();
void chip8_updateInput();
bool chip8_drawRequested();

#endif
