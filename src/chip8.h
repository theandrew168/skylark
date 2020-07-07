#ifndef SKYLARK_CHIP8_H_INCLUDED
#define SKYLARK_CHIP8_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

enum {
    CHIP8_MEM_SIZE = 4096,
    CHIP8_REG_SIZE = 16,
    CHIP8_STACK_SIZE = 16,
    CHIP8_INPUT_SIZE = 16,
    CHIP8_DISPLAY_WIDTH = 64,
    CHIP8_DISPLAY_HEIGHT = 32,
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

enum chip8_status {
    CHIP8_OK = 0,
    CHIP8_ERROR_OVERSIZED_ROM,
    CHIP8_ERROR_STACK_OVERFLOW,
    CHIP8_ERROR_STACK_UNDERFLOW,
    CHIP8_ERROR_INVALID_INSTRUCTION,
    CHIP8_ERROR_UNDEFINED_OPERATION,
};

// Low-level emulation API
int chip8_instruction_decode(struct instruction* inst, uint16_t code);
const char* chip8_instruction_name(const struct instruction* inst);
int chip8_operation_apply(struct chip8* chip8, const struct instruction* inst);

// High-level emulation API
int chip8_init(struct chip8* chip8);
int chip8_load(struct chip8* chip8, const uint8_t* rom, long size);
int chip8_step(struct chip8* chip8);
bool chip8_pixel_on(const struct chip8* chip8, long x, long y);
const char* chip8_error_message(int error);

#endif
