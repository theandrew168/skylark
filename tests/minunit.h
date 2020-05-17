#ifndef MINUNIT_H_INCLUDED
#define MINUNIT_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define mu_assert(message, test) do { if (!(test)) return message#test; } while (0)
#define mu_run_test(test) do { const char *message = test(); tests_run++;  \
                               if (message) return message; } while (0)
#define mu_run_test_suite(suite) do { const char *message = suite();  \
                                      if (message) return message; } while (0)
extern int tests_run;

#endif
