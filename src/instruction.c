#include <stdint.h>
#include <stdio.h>

#include "instruction.h"

static const struct {
    uint16_t mask;
    uint16_t value;
} INSTRUCTION_MASKS[] = {
    [OPCODE_UNDEFINED] = { 0, 0 },
    [OPCODE_CLS_00E0]  = { 0xffff, 0x00E0 },
    [OPCODE_RET_00EE]  = { 0xffff, 0x00EE },
    [OPCODE_SYS_0nnn]  = { 0xf000, 0x0000 },
    [OPCODE_JP_1nnn]   = { 0xf000, 0x1000 },
    [OPCODE_CALL_2nnn] = { 0xf000, 0x2000 },
    [OPCODE_SE_3xkk]   = { 0xf000, 0x3000 },
    [OPCODE_SNE_4xkk]  = { 0xf000, 0x4000 },
    [OPCODE_SE_5xy0]   = { 0xf00f, 0x5000 },
    [OPCODE_LD_6xkk]   = { 0xf000, 0x6000 },
    [OPCODE_ADD_7xkk]  = { 0xf000, 0x7000 },
    [OPCODE_LD_8xy0]   = { 0xf00f, 0x8000 },
    [OPCODE_OR_8xy1]   = { 0xf00f, 0x8001 },
    [OPCODE_AND_8xy2]  = { 0xf00f, 0x8002 },
    [OPCODE_XOR_8xy3]  = { 0xf00f, 0x8003 },
    [OPCODE_ADD_8xy4]  = { 0xf00f, 0x8004 },
    [OPCODE_SUB_8xy5]  = { 0xf00f, 0x8005 },
    [OPCODE_SHR_8xy6]  = { 0xf00f, 0x8006 },
    [OPCODE_SUBN_8xy7] = { 0xf00f, 0x8007 },
    [OPCODE_SHL_8xyE]  = { 0xf00f, 0x800E },
    [OPCODE_SNE_9xy0]  = { 0xf00f, 0x9000 },
    [OPCODE_LD_Annn]   = { 0xf000, 0xA000 },
    [OPCODE_JP_Bnnn]   = { 0xf000, 0xB000 },
    [OPCODE_RND_Cxkk]  = { 0xf000, 0xC000 },
    [OPCODE_DRW_Dxyn]  = { 0xf000, 0xD000 },
    [OPCODE_SKP_Ex9E]  = { 0xf0ff, 0xE09E },
    [OPCODE_SKNP_ExA1] = { 0xf0ff, 0xE0A1 },
    [OPCODE_LD_Fx07]   = { 0xf0ff, 0xF007 },
    [OPCODE_LD_Fx0A]   = { 0xf0ff, 0xF00A },
    [OPCODE_LD_Fx15]   = { 0xf0ff, 0xF015 },
    [OPCODE_LD_Fx18]   = { 0xf0ff, 0xF018 },
    [OPCODE_ADD_Fx1E]  = { 0xf0ff, 0xF01E },
    [OPCODE_LD_Fx29]   = { 0xf0ff, 0xF029 },
    [OPCODE_LD_Fx33]   = { 0xf0ff, 0xF033 },
    [OPCODE_LD_Fx55]   = { 0xf0ff, 0xF055 },
    [OPCODE_LD_Fx65]   = { 0xf0ff, 0xF065 },
};

static const char* INSTRUCTION_NAMES[] = {
    [OPCODE_UNDEFINED] = "OPCODE_UNDEFINED",
    [OPCODE_CLS_00E0]  = "OPCODE_CLS_00E0", 
    [OPCODE_RET_00EE]  = "OPCODE_RET_00EE", 
    [OPCODE_SYS_0nnn]  = "OPCODE_SYS_0nnn", 
    [OPCODE_JP_1nnn]   = "OPCODE_JP_1nnn", 
    [OPCODE_CALL_2nnn] = "OPCODE_CALL_2nnn", 
    [OPCODE_SE_3xkk]   = "OPCODE_SE_3xkk", 
    [OPCODE_SNE_4xkk]  = "OPCODE_SNE_4xkk", 
    [OPCODE_SE_5xy0]   = "OPCODE_SE_5xy0", 
    [OPCODE_LD_6xkk]   = "OPCODE_LD_6xkk", 
    [OPCODE_ADD_7xkk]  = "OPCODE_ADD_7xkk", 
    [OPCODE_LD_8xy0]   = "OPCODE_LD_8xy0", 
    [OPCODE_OR_8xy1]   = "OPCODE_OR_8xy1", 
    [OPCODE_AND_8xy2]  = "OPCODE_AND_8xy2", 
    [OPCODE_XOR_8xy3]  = "OPCODE_XOR_8xy3", 
    [OPCODE_ADD_8xy4]  = "OPCODE_ADD_8xy4", 
    [OPCODE_SUB_8xy5]  = "OPCODE_SUB_8xy5", 
    [OPCODE_SHR_8xy6]  = "OPCODE_SHR_8xy6", 
    [OPCODE_SUBN_8xy7] = "OPCODE_SUBN_8xy7", 
    [OPCODE_SHL_8xyE]  = "OPCODE_SHL_8xyE", 
    [OPCODE_SNE_9xy0]  = "OPCODE_SNE_9xy0", 
    [OPCODE_LD_Annn]   = "OPCODE_LD_Annn", 
    [OPCODE_JP_Bnnn]   = "OPCODE_JP_Bnnn", 
    [OPCODE_RND_Cxkk]  = "OPCODE_RND_Cxkk", 
    [OPCODE_DRW_Dxyn]  = "OPCODE_DRW_Dxyn", 
    [OPCODE_SKP_Ex9E]  = "OPCODE_SKP_Ex9E", 
    [OPCODE_SKNP_ExA1] = "OPCODE_SKNP_ExA1", 
    [OPCODE_LD_Fx07]   = "OPCODE_LD_Fx07", 
    [OPCODE_LD_Fx0A]   = "OPCODE_LD_Fx0A", 
    [OPCODE_LD_Fx15]   = "OPCODE_LD_Fx15", 
    [OPCODE_LD_Fx18]   = "OPCODE_LD_Fx18", 
    [OPCODE_ADD_Fx1E]  = "OPCODE_ADD_Fx1E", 
    [OPCODE_LD_Fx29]   = "OPCODE_LD_Fx29", 
    [OPCODE_LD_Fx33]   = "OPCODE_LD_Fx33", 
    [OPCODE_LD_Fx55]   = "OPCODE_LD_Fx55", 
    [OPCODE_LD_Fx65]   = "OPCODE_LD_Fx65", 
};

int
instruction_decode(struct instruction* inst, uint16_t code)
{
    // params can be set now since their position is consistent
    inst->opcode = OPCODE_UNDEFINED;
    inst->nnn = code & 0x0fff;
    inst->n = code & 0x000f;
    inst->x = (code & 0x0f00) >> 8;
    inst->y = (code & 0x00f0) >> 4;
    inst->kk = code & 0x00ff;

    // loop through each opcode and try to find a match
    for (int op = OPCODE_UNDEFINED + 1; op < OPCODE_COUNT; op++) {
        // check for a match by masking the code and comparing the values
        if ((code & INSTRUCTION_MASKS[op].mask) == INSTRUCTION_MASKS[op].value) {
            inst->opcode = op;
            return INSTRUCTION_OK;
        }
    }

    // if nothing matched from the above loop then the code is invalid
    return INSTRUCTION_ERROR;
}

const char*
instruction_name(const struct instruction* inst)
{
    if (inst->opcode <= OPCODE_UNDEFINED || inst->opcode >= OPCODE_COUNT) {
        return INSTRUCTION_NAMES[OPCODE_UNDEFINED];
    }

    return INSTRUCTION_NAMES[inst->opcode];
}
