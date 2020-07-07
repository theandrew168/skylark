#ifndef SKYLARK_INSTRUCTION_H_INCLUDED
#define SKYLARK_INSTRUCTION_H_INCLUDED

#include <stdint.h>

enum opcode {
    OPCODE_UNDEFINED = 0,
    OPCODE_CLS_00E0,
    OPCODE_RET_00EE,
    OPCODE_SYS_0nnn,
    OPCODE_JP_1nnn,
    OPCODE_CALL_2nnn,
    OPCODE_SE_3xkk,
    OPCODE_SNE_4xkk,
    OPCODE_SE_5xy0,
    OPCODE_LD_6xkk,
    OPCODE_ADD_7xkk,
    OPCODE_LD_8xy0,
    OPCODE_OR_8xy1,
    OPCODE_AND_8xy2,
    OPCODE_XOR_8xy3,
    OPCODE_ADD_8xy4,
    OPCODE_SUB_8xy5,
    OPCODE_SHR_8xy6,
    OPCODE_SUBN_8xy7,
    OPCODE_SHL_8xyE,
    OPCODE_SNE_9xy0,
    OPCODE_LD_Annn,
    OPCODE_JP_Bnnn,
    OPCODE_RND_Cxkk,
    OPCODE_DRW_Dxyn,
    OPCODE_SKP_Ex9E,
    OPCODE_SKNP_ExA1,
    OPCODE_LD_Fx07,
    OPCODE_LD_Fx0A,
    OPCODE_LD_Fx15,
    OPCODE_LD_Fx18,
    OPCODE_ADD_Fx1E,
    OPCODE_LD_Fx29,
    OPCODE_LD_Fx33,
    OPCODE_LD_Fx55,
    OPCODE_LD_Fx65,
    OPCODE_COUNT,
};

struct instruction {
    int opcode;
    uint16_t nnn;
    uint8_t n;
    uint8_t x;
    uint8_t y;
    uint8_t kk;
};

enum instruction_status {
    INSTRUCTION_OK = 0,
    INSTRUCTION_ERROR,
};

int instruction_decode(struct instruction* inst, uint16_t code);
const char* instruction_name(const struct instruction* inst);

#endif
