#include "test_framework.h"

#include <stdarg.h>
#include <stdio.h>

static int current_test_failed = 0;

void test_suite_init(test_suite* suite) {
    suite->count = 0;
}

void test_suite_add(test_suite* suite, const char* name, test_fn fn) {
    if (suite->count >= sizeof(suite->cases) / sizeof(suite->cases[0])) {
        return;
    }
    suite->cases[suite->count].name = name;
    suite->cases[suite->count].fn   = fn;
    suite->count += 1;
}

void test_mark_failure(const char* file, int line, const char* expr, const char* message, ...) {
    current_test_failed = 1;
    fprintf(stderr, "Assertion failed at %s:%d (%s): ", file, line, expr);

    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);

    fprintf(stderr, "\n");
}

static void test_begin_case(const char* name) {
    (void)name;
    current_test_failed = 0;
}

static int test_case_failed(void) {
    return current_test_failed;
}

int test_suite_run(test_suite* suite) {
    size_t failures = 0;

    for (size_t i = 0; i < suite->count; i++) {
        test_begin_case(suite->cases[i].name);
        suite->cases[i].fn();
        if (test_case_failed()) {
            fprintf(stderr, "[FAIL] %s\n", suite->cases[i].name);
            failures += 1;
        } else {
            printf("[PASS] %s\n", suite->cases[i].name);
        }
    }

    printf("Ran %zu test(s): %zu passed, %zu failed\n", suite->count, suite->count - failures, failures);
    return failures == 0 ? 0 : 1;
}
