#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip8.c"

bool
test_operation_UNDEFINED(void)
{
    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    struct instruction inst = {
        .opcode = OPCODE_CLS_00E0,
    };

    int rc = operation_UNDEFINED(&chip8, &inst);
    if (rc == CHIP8_OK) {
        fprintf(stderr, "operation_UNDEFINED failed to return an error\n");
        return false;
    }

    return true;
}

bool
test_operation_CLS_00E0(void)
{
    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    struct instruction inst = {
        .opcode = OPCODE_CLS_00E0,
    };

    // manually turn on all pixels
    for (long i = 0; i < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; i++) {
        chip8.display[i] = true;
    }

    uint16_t pc_before = chip8.pc;
    int rc = operation_CLS_00E0(&chip8, &inst);
    if (rc != CHIP8_OK) {
        fprintf(stderr, "operation_CLS_00E0 returned an error: %s\n", chip8_error_message(rc));
        return false;
    }

    for (long i = 0; i < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; i++) {
        if (chip8.display[i] == true) {
            fprintf(stderr, "operation_CLS_00E0 failed to clear all pixels\n");
            return false;
        }
    }

    if (chip8.pc != pc_before + 2) {
        fprintf(stderr, "operation_CLS_00E0 failed to increment program counter\n");
        return false;
    }

    return true;
}

bool
test_operation_RET_00EE(void)
{
    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    struct instruction inst = {
        .opcode = OPCODE_RET_00EE,
    };

    // validate that calling RET_00EE with an empty stack returns an error
    chip8.sp = 0;
    int rc = operation_RET_00EE(&chip8, &inst);
    if (rc != CHIP8_ERROR_STACK_UNDERFLOW) {
        fprintf(stderr, "operation_RET_00EE did not underflow the stack\n");
        return false;
    }

    // simulate a CALL_2nnn to setup a valid return
    uint16_t ret_addr = 0x0123;
    chip8.stack[0] = ret_addr;
    chip8.sp += 1;

    rc = operation_RET_00EE(&chip8, &inst);
    if (rc != CHIP8_OK) {
        fprintf(stderr, "operation_RET_00EE returned an error: %s\n", chip8_error_message(rc));
        return false;
    }

    if (chip8.pc != ret_addr + 2) {
        fprintf(stderr, "operation_RET_00EE failed pop stack value into program counter\n");
        return false;
    }

    return true;
}
