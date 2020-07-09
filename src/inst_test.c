#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "inst.c"

bool
test_instruction_decode(void)
{
    const struct {
        uint16_t code;
        struct instruction want;
    } tests[] = {
        { .code = 0x00E0, .want = { .opcode = OPCODE_CLS_00E0 }},
        { .code = 0x00EE, .want = { .opcode = OPCODE_RET_00EE }},
        { .code = 0x0234, .want = { .opcode = OPCODE_SYS_0nnn, .nnn = 0x234 }},
        { .code = 0x1234, .want = { .opcode = OPCODE_JP_1nnn, .nnn = 0x234 }},
        { .code = 0x2234, .want = { .opcode = OPCODE_CALL_2nnn, .nnn = 0x234 }},
        { .code = 0x3234, .want = { .opcode = OPCODE_SE_3xkk, .x = 0x2, .kk = 0x34 }},
        { .code = 0x4234, .want = { .opcode = OPCODE_SNE_4xkk, .x = 0x2, .kk = 0x34 }},
        { .code = 0x5230, .want = { .opcode = OPCODE_SE_5xy0, .x = 0x2, .y = 0x3 }},
        { .code = 0x6234, .want = { .opcode = OPCODE_LD_6xkk, .x = 0x2, .kk = 0x34 }},
        { .code = 0x7234, .want = { .opcode = OPCODE_ADD_7xkk, .x = 0x2, .kk = 0x34 }},
        { .code = 0x8230, .want = { .opcode = OPCODE_LD_8xy0, .x = 0x2, .y = 0x3 }},
        { .code = 0x8231, .want = { .opcode = OPCODE_OR_8xy1, .x = 0x2, .y = 0x3 }},
        { .code = 0x8232, .want = { .opcode = OPCODE_AND_8xy2, .x = 0x2, .y = 0x3 }},
        { .code = 0x8233, .want = { .opcode = OPCODE_XOR_8xy3, .x = 0x2, .y = 0x3 }},
        { .code = 0x8234, .want = { .opcode = OPCODE_ADD_8xy4, .x = 0x2, .y = 0x3 }},
        { .code = 0x8235, .want = { .opcode = OPCODE_SUB_8xy5, .x = 0x2, .y = 0x3 }},
        { .code = 0x8236, .want = { .opcode = OPCODE_SHR_8xy6, .x = 0x2, .y = 0x3 }},
        { .code = 0x8237, .want = { .opcode = OPCODE_SUBN_8xy7, .x = 0x2, .y = 0x3 }},
        { .code = 0x823E, .want = { .opcode = OPCODE_SHL_8xyE, .x = 0x2, .y = 0x3 }},
        { .code = 0x9230, .want = { .opcode = OPCODE_SNE_9xy0, .x = 0x2, .y = 0x3 }},
        { .code = 0xA234, .want = { .opcode = OPCODE_LD_Annn, .nnn = 0x234 }},
        { .code = 0xB234, .want = { .opcode = OPCODE_JP_Bnnn, .nnn = 0x234 }},
        { .code = 0xC234, .want = { .opcode = OPCODE_RND_Cxkk, .x = 0x2, .kk = 0x34 }},
        { .code = 0xD234, .want = { .opcode = OPCODE_DRW_Dxyn, .x = 0x2, .y = 0x3, .n = 0x4 }},
        { .code = 0xE29E, .want = { .opcode = OPCODE_SKP_Ex9E, .x = 0x2 }},
        { .code = 0xE2A1, .want = { .opcode = OPCODE_SKNP_ExA1, .x = 0x2 }},
        { .code = 0xF207, .want = { .opcode = OPCODE_LD_Fx07, .x = 0x2 }},
        { .code = 0xF20A, .want = { .opcode = OPCODE_LD_Fx0A, .x = 0x2 }},
        { .code = 0xF215, .want = { .opcode = OPCODE_LD_Fx15, .x = 0x2 }},
        { .code = 0xF218, .want = { .opcode = OPCODE_LD_Fx18, .x = 0x2 }},
        { .code = 0xF21E, .want = { .opcode = OPCODE_ADD_Fx1E, .x = 0x2 }},
        { .code = 0xF229, .want = { .opcode = OPCODE_LD_Fx29, .x = 0x2 }},
        { .code = 0xF233, .want = { .opcode = OPCODE_LD_Fx33, .x = 0x2 }},
        { .code = 0xF255, .want = { .opcode = OPCODE_LD_Fx55, .x = 0x2 }},
        { .code = 0xF265, .want = { .opcode = OPCODE_LD_Fx65, .x = 0x2 }},
    };
    long num_tests = sizeof(tests) / sizeof(tests[0]);

    struct instruction inst = { 0 };
    for (long i = 0; i < num_tests; i++) {
        int rc = instruction_decode(&inst, tests[i].code);
        if (rc != INSTRUCTION_OK) {
            fprintf(stderr, "failed to decode instruction: %04x\n", tests[i].code);
            return false;
        }

        if (inst.opcode != tests[i].want.opcode) {
            fprintf(stderr, "want %d; got %d\n", tests[i].want.opcode, inst.opcode);
            return false;
        }
    }

    return true;
}
