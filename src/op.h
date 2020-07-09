#ifndef SKYLARK_OP_H_INCLUDED
#define SKYLARK_OP_H_INCLUDED

#include "chip8.h"
#include "inst.h"

enum operation_status {
    OPERATION_OK = 0,
    OPERATION_ERROR_INVALID_INSTRUCTION,
    OPERATION_ERROR_UNDEFINED_OPERATION,
    OPERATION_ERROR_STACK_OVERFLOW,
    OPERATION_ERROR_STACK_UNDERFLOW,
};

int operation_apply(struct chip8* chip8, const struct instruction* inst);
const char* operation_error_message(int error);

#endif
