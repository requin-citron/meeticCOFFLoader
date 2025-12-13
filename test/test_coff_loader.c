#include "test_framework.h"

#include <string.h>

#include "beacon_compatibility.h"
#include "coff_loader.h"
#include "windows_stub_helpers.h"

static void clear_beacon_output(void) {
    INT out_size = 0;
    PCHAR buffer = beacon_get_output_data(&out_size);
    if (buffer != NULL) {
        mcfree(buffer);
    }
}

static void test_coff_symbol_helper_functions(void) {
    COFF_SYM symbol = {0};
    symbol.section_number = 1;
    ASSERT_TRUE(coff_symbol_is_defined(&symbol));

    symbol.section_number = 0;
    symbol.storage_class = IMAGE_SYM_CLASS_EXTERNAL;
    ASSERT_TRUE(coff_symbol_is_external(&symbol));

    symbol.storage_class = 0;
    ASSERT_FALSE(coff_symbol_is_external(&symbol));
}

static void test_process_symbol_returns_internal_pointer(void) {
    windows_stub_reset();
    PVOID resolved = process_symbol(PREPENDSYMBOLVALUE "BeaconPrintf");
    ASSERT_PTR_EQ((PVOID)beacon_printf, resolved);
}

static void test_process_symbol_resolves_external_symbols(void) {
    windows_stub_reset();
    windows_stub_set_module_handle_result(NULL);
    windows_stub_set_load_library_result((HMODULE)0x2000);
    windows_stub_set_next_proc_address((FARPROC)0x4545);

    PVOID resolved = process_symbol(PREPENDSYMBOLVALUE "kernel32$GetVersion@0");
    ASSERT_PTR_EQ((PVOID)0x4545, resolved);

    windows_stub_observations obs = windows_stub_last_observations();
    ASSERT_STR_EQ("kernel32", obs.load_library);
    ASSERT_STR_EQ("GetVersion", obs.proc_lookup);
}

static void test_beacon_get_output_data_resets_buffer(void) {
    windows_stub_reset();
    clear_beacon_output();

    beacon_printf(CALLBACK_OUTPUT, "unit");
    INT out_size = 0;
    PCHAR out = beacon_get_output_data(&out_size);
    ASSERT_EQ_INT(4, out_size);
    ASSERT_MEM_EQ("unit", out, 4);
    mcfree(out);

    PCHAR second = beacon_get_output_data(&out_size);
    ASSERT_PTR_EQ(NULL, second);
    ASSERT_EQ_INT(0, out_size);
}

void register_loader_tests(test_suite* suite) {
    test_suite_add(suite, "coff_symbol_helper_functions", test_coff_symbol_helper_functions);
    test_suite_add(suite, "process_symbol_returns_internal_pointer", test_process_symbol_returns_internal_pointer);
    test_suite_add(suite, "process_symbol_resolves_external_symbols", test_process_symbol_resolves_external_symbols);
    test_suite_add(suite, "beacon_get_output_data_resets_buffer", test_beacon_get_output_data_resets_buffer);
}
