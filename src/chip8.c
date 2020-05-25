#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8.h"
#include "isa.h"

int
chip8_init(struct chip8* chip8, const uint8_t* rom, long size)
{
    assert(chip8 != NULL);
    assert(rom != NULL);

    if (CHIP8_ROM_OFFSET + size >= CHIP8_MEM_SIZE) {
        fprintf(stderr, "rom is too large\n");
        return CHIP8_ERROR;
    }

    memset(chip8, 0, sizeof(*chip8));
    memmove(chip8->mem + CHIP8_ROM_OFFSET, rom, size);

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
    case OP_LD_6xkk:
    case OP_ADD_7xkk:
    case OP_LD_8xy0:
    case OP_OR_8xy1:
    case OP_AND_8xy2:
    case OP_XOR_8xy3:
    case OP_ADD_8xy4:
    case OP_SUB_8xy5:
    case OP_SHR_8xy6:
    case OP_SUBN_8xy7:
    case OP_SHL_8xyE:
    case OP_SNE_9xy0:
    case OP_LD_Annn:
    case OP_JP_Bnnn:
    case OP_RND_Cxkk:
    case OP_DRW_Dxyn:
    case OP_SKP_Ex9E:
    case OP_SKNP_ExA1:
    case OP_LD_Fx07:
    case OP_LD_Fx0A:
    case OP_LD_Fx15:
    case OP_LD_Fx18:
    case OP_ADD_Fx1E:
    case OP_LD_Fx29:
    case OP_LD_Fx33:
    case OP_LD_Fx55:
    case OP_LD_Fx65:
    default:
        fprintf(stderr, "invalid instruction found in rom: 0x%04x\n", code);
        return CHIP8_ERROR;
    }

    return CHIP8_OK;
}
