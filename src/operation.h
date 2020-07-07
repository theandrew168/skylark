#ifndef SKYLARK_OPERATION_H_INCLUDED
#define SKYLARK_OPERATION_H_INCLUDED

struct chip8;
struct instruction;

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
