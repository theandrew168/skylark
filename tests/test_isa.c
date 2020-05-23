#include "minunit.h"

#include "isa.h"

const char*
test_isa_decode_OP_CLS_00E0(void)
{
    uint16_t code = 0x00E0;
    struct instruction inst = { 0 };

    int rc = isa_decode(code, &inst);
    mu_assert("test_isa_decode_OP_CLS_00E0: ", rc == ISA_OK);
    mu_assert("test_isa_decode_OP_CLS_00E0: ", inst.opcode == OP_CLS_00E0);

    return NULL;
}

const char*
test_isa_decode_OP_RET_00EE(void)
{
    uint16_t code = 0x00EE;
    struct instruction inst = { 0 };

    int rc = isa_decode(code, &inst);
    mu_assert("test_isa_decode_OP_RET_00EE: ", rc == ISA_OK);
    mu_assert("test_isa_decode_OP_RET_00EE: ", inst.opcode == OP_RET_00EE);

    return NULL;
}

const char*
test_isa(void)
{
    mu_run_test(test_isa_decode_OP_CLS_00E0);
    mu_run_test(test_isa_decode_OP_RET_00EE);
    return NULL;
}
