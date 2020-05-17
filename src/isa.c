#include <stdint.h>

#include "isa.h"

int
isa_decode(uint16_t code, struct instruction* inst)
{
    if (code == 0x00E0) {
        inst->opcode = OP_CLS_00E0;
    } else if (code == 0x00EE) {
        inst->opcode = OP_RET_00EE;
    } else {
        return ISA_ERROR;
    }

    return ISA_OK;
}
