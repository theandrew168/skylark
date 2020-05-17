#ifndef SKYLARK_ISA_H_INCLUDED
#define SKYLARK_ISA_H_INCLUDED

#include <stdint.h>

enum opcode {
    OP_UNDEFINED = 0,
    OP_CLS_00E0,
    OP_RET_00EE,
    OP_SYS_0nnn,
    OP_JP_1nnn,
    OP_CALL_2nnn,
    OP_SE_3xkk,
    OP_SNE_4xkk,
    OP_SE_5xy0,
    OP_LD_6xkk,
    OP_ADD_7xkk,
    OP_LD_8xy0,
    OP_OR_8xy1,
    OP_AND_8xy2,
    OP_XOR_8xy3,
    OP_ADD_8xy4,
    OP_SUB_8xy5,
    OP_SHR_8xy6,
    OP_SUBN_8xy7,
    OP_SHL_8xyE,
    OP_SNE_9xy0,
    OP_LD_Annn,
    OP_JP_Bnnn,
    OP_RND_Cxkk,
    OP_DRW_Dxyn,
    OP_SKP_Ex9E,
    OP_SKNP_ExA1,
    OP_LD_Fx07,
    OP_LD_Fx0A,
    OP_LD_Fx15,
    OP_LD_Fx18,
    OP_ADD_Fx1E,
    OP_LD_Fx29,
    OP_LD_Fx33,
    OP_LD_Fx55,
    OP_LD_Fx65,
};

struct instruction {
    enum opcode opcode;
    uint16_t nnn;
    uint8_t n;
    uint8_t x;
    uint8_t y;
    uint8_t kk;
};

enum isa_status {
    ISA_OK = 0,
    ISA_ERROR,
};

int isa_decode(uint16_t code, struct instruction* inst);

#endif
