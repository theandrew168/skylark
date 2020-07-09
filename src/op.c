#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "inst.h"
#include "op.h"

typedef int (*operation_func)(struct chip8* chip8, const struct instruction* inst);

static int
operation_UNDEFINED(struct chip8* chip8, const struct instruction* inst)
{
    return OPERATION_ERROR_UNDEFINED_OPERATION;
}

static int
operation_CLS_00E0(struct chip8* chip8, const struct instruction* inst)
{
    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_RET_00EE(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->sp <= 0) return OPERATION_ERROR_STACK_UNDERFLOW;

    chip8->sp -= 1;
    chip8->pc = chip8->stack[chip8->sp];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SYS_0nnn(struct chip8* chip8, const struct instruction* inst)
{
    chip8->pc = inst->nnn;
    return OPERATION_OK;
}

static int
operation_JP_1nnn(struct chip8* chip8, const struct instruction* inst)
{
    chip8->pc = inst->nnn;
    return OPERATION_OK;
}

static int
operation_CALL_2nnn(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->sp >= CHIP8_STACK_SIZE - 1) return OPERATION_ERROR_STACK_OVERFLOW;

    chip8->stack[chip8->sp] = chip8->pc;
    chip8->sp += 1;
    chip8->pc = inst->nnn;
    return OPERATION_OK;
}

static int
operation_SE_3xkk(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->reg[inst->x] == inst->kk) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SNE_4xkk(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->reg[inst->x] != inst->kk) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SE_5xy0(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->reg[inst->x] == chip8->reg[inst->y]) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_6xkk(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] = inst->kk;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_ADD_7xkk(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] += inst->kk;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_8xy0(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] = chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_OR_8xy1(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] |= chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_AND_8xy2(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] &= chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_XOR_8xy3(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] ^= chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_ADD_8xy4(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst->x] + chip8->reg[inst->y] > 0xff) ? 1 : 0;
    chip8->reg[inst->x] += chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SUB_8xy5(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst->x] > chip8->reg[inst->y]) ? 1 : 0;
    chip8->reg[inst->x] -= chip8->reg[inst->y];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SHR_8xy6(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst->x] & 0x1) ? 1 : 0;
    chip8->reg[inst->x] >>= 1;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SUBN_8xy7(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst->y] > chip8->reg[inst->x]) ? 1 : 0;
    chip8->reg[inst->x] = chip8->reg[inst->y] - chip8->reg[inst->x];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SHL_8xyE(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = (chip8->reg[inst->x] & 0x8) ? 1 : 0;
    chip8->reg[inst->x] <<= 1;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SNE_9xy0(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->reg[inst->x] != chip8->reg[inst->y]) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Annn(struct chip8* chip8, const struct instruction* inst)
{
    chip8->index = inst->nnn;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_JP_Bnnn(struct chip8* chip8, const struct instruction* inst)
{
    chip8->pc = inst->nnn + chip8->reg[CHIP8_REG_V0];
    return OPERATION_OK;
}

static int
operation_RND_Cxkk(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] = rand() & inst->kk;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_DRW_Dxyn(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[CHIP8_REG_VF] = 0;
    for (long dy = 0; dy < inst->n; dy++) {
        uint8_t sprite = chip8->mem[chip8->index + dy];
        for (long dx = 0; dx < CHIP8_SPRITE_WIDTH; dx++) {
            // account for pixels wrapping around edges of the display
            long x = (chip8->reg[inst->x] + dx) % CHIP8_DISPLAY_WIDTH;
            long y = (chip8->reg[inst->y] + dy) % CHIP8_DISPLAY_HEIGHT;

            // chip8->display is stored as a row-major matrix
            long pixel_index = (y * CHIP8_DISPLAY_WIDTH) + x;

            // determine what the resulting pixel value will be
            bool new_pixel = sprite & (0x80 >> dx);
            bool old_pixel = chip8->display[pixel_index];
            bool resulting_pixel = new_pixel ^ old_pixel;

            // set VF register to 1 if a pixel gets turned off
            if (old_pixel && !resulting_pixel) chip8->reg[CHIP8_REG_VF] = 1;

            chip8->display[pixel_index] = resulting_pixel;
        }
    }
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SKP_Ex9E(struct chip8* chip8, const struct instruction* inst)
{
    if (chip8->input[chip8->reg[inst->x]]) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_SKNP_ExA1(struct chip8* chip8, const struct instruction* inst)
{
    if (!chip8->input[chip8->reg[inst->x]]) chip8->pc += 2;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx07(struct chip8* chip8, const struct instruction* inst)
{
    chip8->reg[inst->x] = chip8->timer_delay;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx0A(struct chip8* chip8, const struct instruction* inst)
{
    for (long i = 0; i < CHIP8_INPUT_SIZE; i++) {
        if (chip8->input[i]) {
            chip8->reg[inst->x] = i;
            chip8->pc += 2;
            break;
        }
    }
    // no "pc += 2" here means that this instruction
    // will loop until an input key is pressed
    return OPERATION_OK;
}

static int
operation_LD_Fx15(struct chip8* chip8, const struct instruction* inst)
{
    chip8->timer_delay = chip8->reg[inst->x];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx18(struct chip8* chip8, const struct instruction* inst)
{
    chip8->timer_sound = chip8->reg[inst->x];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_ADD_Fx1E(struct chip8* chip8, const struct instruction* inst)
{
    chip8->index += chip8->reg[inst->x];
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx29(struct chip8* chip8, const struct instruction* inst)
{
    chip8->index = chip8->reg[inst->x] * 5;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx33(struct chip8* chip8, const struct instruction* inst)
{
    chip8->mem[chip8->index + 0] = (chip8->reg[inst->x] / 100);
    chip8->mem[chip8->index + 1] = (chip8->reg[inst->x] / 10)  % 10;
    chip8->mem[chip8->index + 2] = (chip8->reg[inst->x] / 100) % 10;
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx55(struct chip8* chip8, const struct instruction* inst)
{
    for (long i = 0; i < inst->x; i++) {
        chip8->mem[chip8->index + i] = chip8->reg[i];
    }
    chip8->pc += 2;
    return OPERATION_OK;
}

static int
operation_LD_Fx65(struct chip8* chip8, const struct instruction* inst)
{
    for (long i = 0; i < inst->x; i++) {
        chip8->reg[i] = chip8->mem[chip8->index + i];
    }
    chip8->pc += 2;
    return OPERATION_OK;
}

static operation_func OPERATIONS[] = {
    [OPCODE_UNDEFINED] = operation_UNDEFINED,
    [OPCODE_CLS_00E0] = operation_CLS_00E0,
    [OPCODE_RET_00EE] = operation_RET_00EE,
    [OPCODE_SYS_0nnn] = operation_SYS_0nnn,
    [OPCODE_JP_1nnn] = operation_JP_1nnn,
    [OPCODE_CALL_2nnn] = operation_CALL_2nnn,
    [OPCODE_SE_3xkk] = operation_SE_3xkk,
    [OPCODE_SNE_4xkk] = operation_SNE_4xkk,
    [OPCODE_SE_5xy0] = operation_SE_5xy0,
    [OPCODE_LD_6xkk] = operation_LD_6xkk,
    [OPCODE_ADD_7xkk] = operation_ADD_7xkk,
    [OPCODE_LD_8xy0] = operation_LD_8xy0,
    [OPCODE_OR_8xy1] = operation_OR_8xy1,
    [OPCODE_AND_8xy2] = operation_AND_8xy2,
    [OPCODE_XOR_8xy3] = operation_XOR_8xy3,
    [OPCODE_ADD_8xy4] = operation_ADD_8xy4,
    [OPCODE_SUB_8xy5] = operation_SUB_8xy5,
    [OPCODE_SHR_8xy6] = operation_SHR_8xy6,
    [OPCODE_SUBN_8xy7] = operation_SUBN_8xy7,
    [OPCODE_SHL_8xyE] = operation_SHL_8xyE,
    [OPCODE_SNE_9xy0] = operation_SNE_9xy0,
    [OPCODE_LD_Annn] = operation_LD_Annn,
    [OPCODE_JP_Bnnn] = operation_JP_Bnnn,
    [OPCODE_RND_Cxkk] = operation_RND_Cxkk,
    [OPCODE_DRW_Dxyn] = operation_DRW_Dxyn,
    [OPCODE_SKP_Ex9E] = operation_SKP_Ex9E,
    [OPCODE_SKNP_ExA1] = operation_SKNP_ExA1,
    [OPCODE_LD_Fx07] = operation_LD_Fx07,
    [OPCODE_LD_Fx0A] = operation_LD_Fx0A,
    [OPCODE_LD_Fx15] = operation_LD_Fx15,
    [OPCODE_LD_Fx18] = operation_LD_Fx18,
    [OPCODE_ADD_Fx1E] = operation_ADD_Fx1E,
    [OPCODE_LD_Fx29] = operation_LD_Fx29,
    [OPCODE_LD_Fx33] = operation_LD_Fx33,
    [OPCODE_LD_Fx55] = operation_LD_Fx55,
    [OPCODE_LD_Fx65] = operation_LD_Fx65,
};

int
operation_apply(struct chip8* chip8, const struct instruction* inst)
{
    assert(chip8 != NULL);
    assert(inst != NULL);

    operation_func operation = OPERATIONS[inst->opcode];
    return operation(chip8, inst);
}

const char*
operation_error_message(int error)
{
    switch (error) {
    case OPERATION_OK: return "OK";
    case OPERATION_ERROR_INVALID_INSTRUCTION: return "invalid instruction";
    case OPERATION_ERROR_UNDEFINED_OPERATION: return "undefined operation";
    case OPERATION_ERROR_STACK_OVERFLOW: return "stack overflow";
    case OPERATION_ERROR_STACK_UNDERFLOW: return "stack underflow";
    default: return "unknown error";
    }
}
