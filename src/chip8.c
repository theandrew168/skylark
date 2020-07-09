#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8.h"
#include "inst.h"
#include "op.h"

// Chip8 font information
// 'A', for example:
// 11110000
// 10010000
// 11110000
// 10010000
// 10010000
static const uint8_t CHIP8_FONT[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xf0, 0x10, 0xf0, 0x80, 0xf0, /* 2 */
    0xf0, 0x10, 0xf0, 0x10, 0xf0, /* 3 */
    0x90, 0x90, 0xf0, 0x10, 0x10, /* 4 */
    0xf0, 0x80, 0xf0, 0x10, 0xf0, /* 5 */
    0xf0, 0x80, 0xf0, 0x90, 0xf0, /* 6 */
    0xf0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xf0, 0x90, 0xf0, 0x90, 0xf0, /* 8 */
    0xf0, 0x90, 0xf0, 0x10, 0xf0, /* 9 */
    0xf0, 0x90, 0xf0, 0x90, 0x90, /* A */
    0xe0, 0x90, 0xe0, 0x90, 0xe0, /* B */
    0xf0, 0x80, 0x80, 0x80, 0xf0, /* C */
    0xe0, 0x90, 0x90, 0x90, 0xe0, /* D */
    0xf0, 0x80, 0xf0, 0x80, 0xf0, /* E */
    0xf0, 0x80, 0xf0, 0x80, 0x80  /* F */
};

int
chip8_init(struct chip8* chip8)
{
    assert(chip8 != NULL);

    memset(chip8, 0, sizeof(*chip8));
    srand(time(NULL));

    memmove(chip8->mem, CHIP8_FONT, sizeof(CHIP8_FONT));

    return CHIP8_OK;
}

int
chip8_load(struct chip8* chip8, const uint8_t* rom, long size)
{
    assert(rom != NULL);

    if (CHIP8_ROM_ADDR + size >= CHIP8_MEM_SIZE) {
        return CHIP8_ERROR_OVERSIZED_ROM;
    }

    memmove(chip8->mem + CHIP8_ROM_ADDR, rom, size);
    chip8->pc = CHIP8_ROM_ADDR;

    return CHIP8_OK;
}

int
chip8_step(struct chip8* chip8)
{
    uint16_t code = chip8->mem[chip8->pc] << 8 | chip8->mem[chip8->pc + 1];

    struct instruction inst = { 0 };
    int rc = instruction_decode(&inst, code);
    if (rc != INSTRUCTION_OK) {
        fprintf(stderr, "attempted to decode a bad instruction\n");
        return CHIP8_ERROR_BAD_INSTRUCTION;
    }

    rc = operation_apply(chip8, &inst);
    if (rc != OPERATION_OK) {
        fprintf(stderr, "attempted to execute a bad operation: %s\n", operation_error_message(rc));
        return CHIP8_ERROR_BAD_OPERATION;
    }

    if (chip8->timer_delay > 0) chip8->timer_delay -= 1;
    if (chip8->timer_sound > 0) chip8->timer_sound -= 1;

    return CHIP8_OK;
}

bool
chip8_pixel_on(const struct chip8* chip8, long x, long y)
{
    if (x < 0 || x >= CHIP8_DISPLAY_WIDTH) return false;
    if (y < 0 || y >= CHIP8_DISPLAY_HEIGHT) return false;

    // row-major ordering
    long index = (y * CHIP8_DISPLAY_WIDTH) + x;
    return chip8->display[index];
}
