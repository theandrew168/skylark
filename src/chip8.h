#ifndef SKYLARK_CHIP8_H
#define SKYLARK_CHIP8_H

#include <stdbool.h>
#include <stdint.h>

enum {
    CHIP8_MEM_SIZE = 4096,
    CHIP8_REG_SIZE = 16,
    CHIP8_STACK_SIZE = 16,
    CHIP8_INPUT_SIZE = 16,
    CHIP8_DISPLAY_WIDTH = 64,
    CHIP8_DISPLAY_HEIGHT = 32,
    CHIP8_REG_V0 = 0,
    CHIP8_REG_VF = 15,
    CHIP8_SPRITE_WIDTH = 8,
    CHIP8_FONT_SIZE = 5,
    CHIP8_ROM_ADDR = 512,
};

enum {
    CHIP8_OK = 0,
    CHIP8_ERROR_OVERSIZED_ROM,
    CHIP8_ERROR_BAD_INSTRUCTION,
    CHIP8_ERROR_BAD_OPERATION,
};

struct chip8 {
    uint8_t mem[CHIP8_MEM_SIZE];
    uint8_t reg[CHIP8_REG_SIZE];

    uint16_t index;
    uint16_t pc;

    uint16_t stack[CHIP8_STACK_SIZE];
    uint16_t sp;

    bool input[CHIP8_INPUT_SIZE];
    bool display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

    uint8_t timer_delay;
    uint8_t timer_sound;
};

int chip8_init(struct chip8* chip8);
int chip8_load(struct chip8* chip8, const uint8_t* rom, long size);
int chip8_step(struct chip8* chip8);
bool chip8_pixel(const struct chip8* chip8, long x, long y);

#endif
