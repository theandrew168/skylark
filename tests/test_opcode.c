#include "minunit.h"

const char*
test_opcode_foo(void)
{
    return NULL;
}

const char*
test_opcode(void)
{
    mu_run_test(test_opcode_foo);
    return NULL;
}
