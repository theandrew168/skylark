#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8.h"
#include "isa.h"

enum {
    CHIP8_REG_V0 = 0,
    CHIP8_REG_VF = 15,

    CHIP8_ROM_ADDR = 512,
    CHIP8_SPRITE_WIDTH = 8,
    CHIP8_FONT_SIZE = 5,
};

typedef void (*instruction_func)(struct chip8* chip8, struct instruction* inst);

int
chip8_init(struct chip8* chip8, const uint8_t* rom, long size)
{
    assert(chip8 != NULL);
    assert(rom != NULL);

    if (CHIP8_ROM_ADDR + size >= CHIP8_MEM_SIZE) {
        fprintf(stderr, "rom is too large\n");
        return CHIP8_ERROR;
    }

    memset(chip8, 0, sizeof(*chip8));
    memmove(chip8->mem + CHIP8_ROM_ADDR, rom, size);
    chip8->pc = CHIP8_ROM_ADDR;

    srand(time(NULL));

    return CHIP8_OK;
}

int
chip8_step(struct chip8* chip8)
{
    uint16_t code = chip8->mem[chip8->pc] << 8 | chip8->mem[chip8->pc + 1];

    struct instruction inst = { 0 };
    int rc = isa_instruction_decode(&inst, code);
    if (rc != ISA_OK) {
        fprintf(stderr, "invalid instruction found in rom: 0x%04x\n", code);
        return CHIP8_ERROR;
    }

    switch (inst.opcode) {
    case OP_CLS_00E0:
        memset(chip8->display, 0, sizeof(chip8->display));
        chip8->pc += 2;
        break;
    case OP_RET_00EE:
        chip8->sp -= 1;
        chip8->pc = chip8->stack[chip8->sp];
        chip8->pc += 2;
        break;
    case OP_SYS_0nnn:
    case OP_JP_1nnn:
        chip8->pc = inst.nnn;
        break;
    case OP_CALL_2nnn:
        chip8->stack[chip8->sp] = chip8->pc;
        chip8->sp += 1;
        chip8->pc = inst.nnn;
        break;
    case OP_SE_3xkk:
        if (chip8->reg[inst.x] == inst.kk) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_SNE_4xkk:
        if (chip8->reg[inst.x] != inst.kk) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_SE_5xy0:
        if (chip8->reg[inst.x] == chip8->reg[inst.y]) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_LD_6xkk:
        chip8->reg[inst.x] = inst.kk;
        chip8->pc += 2;
        break;
    case OP_ADD_7xkk:
        chip8->reg[inst.x] += inst.kk;
        chip8->pc += 2;
        break;
    case OP_LD_8xy0:
        chip8->reg[inst.x] = chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_OR_8xy1:
        chip8->reg[inst.x] |= chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_AND_8xy2:
        chip8->reg[inst.x] &= chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_XOR_8xy3:
        chip8->reg[inst.x] ^= chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_ADD_8xy4:
        chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst.x] + chip8->reg[inst.y] > 0xff) ? 1 : 0;
        chip8->reg[inst.x] += chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_SUB_8xy5:
        chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst.x] > chip8->reg[inst.y]) ? 1 : 0;
        chip8->reg[inst.x] -= chip8->reg[inst.y];
        chip8->pc += 2;
        break;
    case OP_SHR_8xy6:
        chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst.x] & 0x1) ? 1 : 0;
        chip8->reg[inst.x] >>= 1;
        chip8->pc += 2;
        break;
    case OP_SUBN_8xy7:
        chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst.y] > chip8->reg[inst.x]) ? 1 : 0;
        chip8->reg[inst.x] = chip8->reg[inst.y] - chip8->reg[inst.x];
        chip8->pc += 2;
        break;
    case OP_SHL_8xyE:
        chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst.x] & 0x8) ? 1 : 0;
        chip8->reg[inst.x] <<= 1;
        chip8->pc += 2;
        break;
    case OP_SNE_9xy0:
        if (chip8->reg[inst.x] != chip8->reg[inst.y]) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_LD_Annn:
        chip8->index = inst.nnn;
        chip8->pc += 2;
        break;
    case OP_JP_Bnnn:
        chip8->pc = inst.nnn + chip8->reg[CHIP8_REG_V0];
        break;
    case OP_RND_Cxkk:
        chip8->reg[inst.x] = rand() & inst.kk;
        chip8->pc += 2;
        break;
    case OP_DRW_Dxyn:
        chip8->reg[CHIP8_REG_VF] = 0;
        for (long dy = 0; dy < inst.n; dy++) {
            uint8_t sprite = chip8->mem[chip8->index + dy];
            for (long dx = 0; dx < CHIP8_SPRITE_WIDTH; dx++) {
                // chip8->display is row-major
                long pixel_index = ((chip8->reg[inst.y] + dy) * CHIP8_DISPLAY_WIDTH)
                                   + chip8->reg[inst.x] + dx;

                bool new_pixel = sprite & (0x80 >> dx);
                bool old_pixel = chip8->display[pixel_index];
                //if (!new_pixel && old_pixel) chip8->reg[CHIP8_REG_VF] = 1;
                if (!(new_pixel ^ old_pixel)) chip8->reg[CHIP8_REG_VF] = 1;

                chip8->display[pixel_index] = new_pixel ^ old_pixel;
            }
        }
        chip8->pc += 2;
        break;
    case OP_SKP_Ex9E:
        if (chip8->input[chip8->reg[inst.x]]) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_SKNP_ExA1:
        if (!chip8->input[chip8->reg[inst.x]]) chip8->pc += 2;
        chip8->pc += 2;
        break;
    case OP_LD_Fx07:
        chip8->reg[inst.x] = chip8->timer_delay;
        chip8->pc += 2;
        break;
    case OP_LD_Fx0A:
        for (long i = 0; i < CHIP8_INPUT_SIZE; i++) {
            if (chip8->input[i]) {
                chip8->reg[inst.x] = i;
                chip8->pc += 2;
                break;
            }
        }
        // no "pc += 2" here means that this instruction
        // will loop until an input key is pressed
        break;
    case OP_LD_Fx15:
        chip8->timer_delay = chip8->reg[inst.x];
        chip8->pc += 2;
        break;
    case OP_LD_Fx18:
        chip8->timer_sound = chip8->reg[inst.x];
        chip8->pc += 2;
        break;
    case OP_ADD_Fx1E:
        chip8->index += chip8->reg[inst.x];
        chip8->pc += 2;
        break;
    case OP_LD_Fx29:
        chip8->index += inst.x * CHIP8_FONT_SIZE;
        chip8->pc += 2;
        break;
    case OP_LD_Fx33:
        chip8->mem[chip8->index + 0] = (chip8->reg[inst.x] / 100);
        chip8->mem[chip8->index + 1] = (chip8->reg[inst.x] / 10)  % 10;
        chip8->mem[chip8->index + 2] = (chip8->reg[inst.x] / 100) % 10;
        chip8->pc += 2;
        break;
    case OP_LD_Fx55:
        for (long i = 0; i < inst.x; i++) {
            chip8->mem[chip8->index + i] = chip8->reg[i];
        }
        chip8->pc += 2;
        break;
    case OP_LD_Fx65:
        for (long i = 0; i < inst.x; i++) {
            chip8->reg[i] = chip8->mem[chip8->index + i];
        }
        chip8->pc += 2;
        break;
    default:
        fprintf(stderr, "invalid instruction found in rom: 0x%04x\n", code);
        return CHIP8_ERROR;
    }

    // TODO: keep these here?
    if (chip8->timer_delay > 0) chip8->timer_delay -= 1;
    if (chip8->timer_sound > 0) chip8->timer_sound -= 1;

    return CHIP8_OK;
}
