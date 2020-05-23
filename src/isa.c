#include <stdint.h>

#include "isa.h"

int
isa_decode(uint16_t code, struct instruction* inst)
{
    // params can be set now since their position is consistent
    inst->nnn = code & 0x0fff;
    inst->n = code & 0x000f;
    inst->x = (code & 0x0f00) >> 8;
    inst->y = (code & 0x00f0) >> 4;
    inst->kk = code & 0x00ff;

    // then just the opcode needs to be determined
    if ((code & 0xffff) == 0x00E0) {
        inst->opcode = OP_CLS_00E0;
    } else if ((code & 0xffff) == 0x00EE) {
        inst->opcode = OP_RET_00EE;
    } else if ((code & 0xf000) == 0x0000) {
        inst->opcode = OP_SYS_0nnn;
    } else {
        return ISA_ERROR;
    }

    return ISA_OK;
}
