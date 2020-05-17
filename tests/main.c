#include "minunit.h"

#include "test_isa.c"

int tests_run = 0;

const char*
test_all(void)
{
    mu_run_test_suite(test_isa);
    return NULL;
}

int
main(int argc, char* argv[])
{
    const char* result = test_all();
    if (result != NULL) {
        printf("TEST FAILED: %s\n", result);
    } else {
        puts("ALL TESTS PASSED");
    }

    printf("Tests run: %d\n", tests_run);

    if (result != NULL) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
