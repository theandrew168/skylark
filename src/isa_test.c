#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "isa.h"

bool
test_isa_instruction_decode(void)
{
    const struct {
        uint16_t code;
        struct instruction want;
    } tests[] = {
        { .code = 0x00E0, .want = { .opcode = OP_CLS_00E0 }},
        { .code = 0x00EE, .want = { .opcode = OP_RET_00EE }},
    };
    long num_tests = sizeof(tests) / sizeof(tests[0]);

    struct instruction inst = { 0 };
    for (long i = 0; i < num_tests; i++) {
        int rc = isa_instruction_decode(&inst, tests[i].code);
        if (rc != ISA_OK) {
            fprintf(stderr, "failed to decode instruction: %04x", tests[i].code);
            return false;
        }

        if (inst.opcode != tests[i].want.opcode) {
            fprintf(stderr, "want %d; got %d", tests[i].want.opcode, inst.opcode);
        }
    }

    return true;
}
