#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "operation.c"

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
        fprintf(stderr, "operation_CLS_00E0 returned an error: %s\n", operation_error_message(rc));
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
    if (rc != OPERATION_ERROR_STACK_UNDERFLOW) {
        fprintf(stderr, "operation_RET_00EE did not underflow the stack\n");
        return false;
    }

    // execute a CALL_2nnn to setup a valid return
    operation_CALL_2nnn(&chip8, &(struct instruction){
        .opcode = OPCODE_CALL_2nnn,
        .nnn = 0x234,
    });

    rc = operation_RET_00EE(&chip8, &inst);
    if (rc != CHIP8_OK) {
        fprintf(stderr, "operation_RET_00EE returned an error: %s\n", operation_error_message(rc));
        return false;
    }

    // pc will be 0x0002 because the CALL_2nnn was made from pc == 0
    if (chip8.pc != 0x0002) {
        fprintf(stderr, "operation_RET_00EE returned to the wrong address\n");
        return false;
    }

    return true;
}

bool
test_operation_SYS_0nnn(void)
{
    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    struct instruction inst = {
        .opcode = OPCODE_SYS_0nnn,
        .nnn = 0x234,
    };

    int rc = operation_SYS_0nnn(&chip8, &inst);
    if (rc != CHIP8_OK) {
        fprintf(stderr, "operation_SYS_0nnn returned an error: %s\n", operation_error_message(rc));
        return false;
    }

    if (chip8.pc != 0x234) {
        fprintf(stderr, "operation_SYS_0nnn set the program counter to the wrong value\n");
        return false;
    }

    return true;
}

bool
test_operation_JP_1nnn(void)
{
    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    struct instruction inst = {
        .opcode = OPCODE_JP_1nnn,
        .nnn = 0x234,
    };

    int rc = operation_JP_1nnn(&chip8, &inst);
    if (rc != CHIP8_OK) {
        fprintf(stderr, "operation_JP_1nnn returned an error: %s\n", operation_error_message(rc));
        return false;
    }

    if (chip8.pc != 0x234) {
        fprintf(stderr, "operation_JP_1nnn set the program counter to the wrong value\n");
        return false;
    }

    return true;
}
