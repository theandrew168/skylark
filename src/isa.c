#include <stdint.h>
#include <stdio.h>

#include "isa.h"

static const struct {
    uint16_t mask;
    uint16_t value;
} OP_MASK_VALUES[] = {
    [OP_UNDEFINED] = { 0, 0 },
    [OP_CLS_00E0]  = { 0xffff, 0x00E0 },
    [OP_RET_00EE]  = { 0xffff, 0x00EE },
    [OP_SYS_0nnn]  = { 0xf000, 0x0000 },
    [OP_JP_1nnn]   = { 0xf000, 0x1000 },
    [OP_CALL_2nnn] = { 0xf000, 0x2000 },
    [OP_SE_3xkk]   = { 0xf000, 0x3000 },
    [OP_SNE_4xkk]  = { 0xf000, 0x4000 },
    [OP_SE_5xy0]   = { 0xf00f, 0x5000 },
    [OP_LD_6xkk]   = { 0xf000, 0x6000 },
    [OP_ADD_7xkk]  = { 0xf000, 0x7000 },
    [OP_LD_8xy0]   = { 0xf00f, 0x8000 },
    [OP_OR_8xy1]   = { 0xf00f, 0x8001 },
    [OP_AND_8xy2]  = { 0xf00f, 0x8002 },
    [OP_XOR_8xy3]  = { 0xf00f, 0x8003 },
    [OP_ADD_8xy4]  = { 0xf00f, 0x8004 },
    [OP_SUB_8xy5]  = { 0xf00f, 0x8005 },
    [OP_SHR_8xy6]  = { 0xf00f, 0x8006 },
    [OP_SUBN_8xy7] = { 0xf00f, 0x8007 },
    [OP_SHL_8xyE]  = { 0xf00f, 0x800E },
    [OP_SNE_9xy0]  = { 0xf00f, 0x9000 },
    [OP_LD_Annn]   = { 0xf000, 0xA000 },
    [OP_JP_Bnnn]   = { 0xf000, 0xB000 },
    [OP_RND_Cxkk]  = { 0xf000, 0xC000 },
    [OP_DRW_Dxyn]  = { 0xf000, 0xD000 },
    [OP_SKP_Ex9E]  = { 0xf0ff, 0xE09E },
    [OP_SKNP_ExA1] = { 0xf0ff, 0xE0A1 },
    [OP_LD_Fx07]   = { 0xf0ff, 0xF007 },
    [OP_LD_Fx0A]   = { 0xf0ff, 0xF00A },
    [OP_LD_Fx15]   = { 0xf0ff, 0xF015 },
    [OP_LD_Fx18]   = { 0xf0ff, 0xF018 },
    [OP_ADD_Fx1E]  = { 0xf0ff, 0xF01E },
    [OP_LD_Fx29]   = { 0xf0ff, 0xF029 },
    [OP_LD_Fx33]   = { 0xf0ff, 0xF033 },
    [OP_LD_Fx55]   = { 0xf0ff, 0xF055 },
    [OP_LD_Fx65]   = { 0xf0ff, 0xF065 },
    [OP_COUNT]     = { 0, 0 },
};

static const char* OP_NAMES[] = {
    [OP_UNDEFINED] = "OP_UNDEFINED",
    [OP_CLS_00E0]  = "OP_TODO", 
    [OP_RET_00EE]  = "OP_TODO", 
    [OP_SYS_0nnn]  = "OP_TODO", 
    [OP_JP_1nnn]   = "OP_TODO", 
    [OP_CALL_2nnn] = "OP_TODO", 
    [OP_SE_3xkk]   = "OP_TODO", 
    [OP_SNE_4xkk]  = "OP_TODO", 
    [OP_SE_5xy0]   = "OP_TODO", 
    [OP_LD_6xkk]   = "OP_TODO", 
    [OP_ADD_7xkk]  = "OP_TODO", 
    [OP_LD_8xy0]   = "OP_TODO", 
    [OP_OR_8xy1]   = "OP_TODO", 
    [OP_AND_8xy2]  = "OP_TODO", 
    [OP_XOR_8xy3]  = "OP_TODO", 
    [OP_ADD_8xy4]  = "OP_TODO", 
    [OP_SUB_8xy5]  = "OP_TODO", 
    [OP_SHR_8xy6]  = "OP_TODO", 
    [OP_SUBN_8xy7] = "OP_TODO", 
    [OP_SHL_8xyE]  = "OP_TODO", 
    [OP_SNE_9xy0]  = "OP_TODO", 
    [OP_LD_Annn]   = "OP_TODO", 
    [OP_JP_Bnnn]   = "OP_TODO", 
    [OP_RND_Cxkk]  = "OP_TODO", 
    [OP_DRW_Dxyn]  = "OP_TODO", 
    [OP_SKP_Ex9E]  = "OP_TODO", 
    [OP_SKNP_ExA1] = "OP_TODO", 
    [OP_LD_Fx07]   = "OP_TODO", 
    [OP_LD_Fx0A]   = "OP_TODO", 
    [OP_LD_Fx15]   = "OP_TODO", 
    [OP_LD_Fx18]   = "OP_TODO", 
    [OP_ADD_Fx1E]  = "OP_TODO", 
    [OP_LD_Fx29]   = "OP_TODO", 
    [OP_LD_Fx33]   = "OP_TODO", 
    [OP_LD_Fx55]   = "OP_TODO", 
    [OP_LD_Fx65]   = "OP_TODO", 
    [OP_COUNT]     = "OP_COUNT", 
};

int
isa_decode(uint16_t code, struct instruction* inst)
{
    // params can be set now since their position is consistent
    inst->opcode = OP_UNDEFINED;
    inst->nnn = code & 0x0fff;
    inst->n = code & 0x000f;
    inst->x = (code & 0x0f00) >> 8;
    inst->y = (code & 0x00f0) >> 4;
    inst->kk = code & 0x00ff;

    // loop through each opcode and try to find a match
    for (int op = OP_UNDEFINED + 1; op < OP_COUNT; op++) {
        // check for a match by masking the code and comparing the values
        if ((code & OP_MASK_VALUES[op].mask) == OP_MASK_VALUES[op].value) {
            inst->opcode = op;
            return ISA_OK;
        }
    }

    // if nothing matched from the above loop then the code is invalid
    return ISA_ERROR;
}

void
isa_print(const struct instruction* inst)
{
    if (inst->opcode <= OP_UNDEFINED || inst->opcode >= OP_COUNT) {
        printf("???\n");
        return;
    }

    const char fmt[] = "op: %s\tnnn: 0x%04x n: 0x%02x x: 0x%02x y: 0x%02x kk: 0x%02x\n";
    printf(fmt, OP_NAMES[inst->opcode], inst->nnn, inst->n, inst->x, inst->y, inst->kk);
}
