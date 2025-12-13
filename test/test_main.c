#include "test_framework.h"

#include "windows_stub_helpers.h"

void register_beacon_function_tests(test_suite* suite);
void register_loader_tests(test_suite* suite);

int main(void) {
    windows_stub_reset();

    test_suite suite;
    test_suite_init(&suite);

    register_beacon_function_tests(&suite);
    register_loader_tests(&suite);

    return test_suite_run(&suite);
}
