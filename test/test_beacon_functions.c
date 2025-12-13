#include "test_framework.h"

#include <stdint.h>
#include <string.h>
#include <wchar.h>

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

static void test_beacon_data_parse_sets_initial_state(void) {
    unsigned char raw[] = {0, 0, 0, 0, 1, 2, 3, 4};
    datap parser = {0};

    beacon_data_parse(&parser, (char*)raw, sizeof(raw));

    ASSERT_PTR_EQ(raw, parser.original);
    ASSERT_PTR_EQ(raw + 4, parser.buffer);
    ASSERT_EQ_INT(4, parser.length);
    ASSERT_EQ_INT(4, parser.size);
}

static void test_beacon_data_int_and_short_consume_stream(void) {
    unsigned char raw[] = {
        0, 0, 0, 0,
        0x44, 0x33, 0x22, 0x11,
        0x02, 0x01
    };
    datap parser = {0};

    beacon_data_parse(&parser, (char*)raw, sizeof(raw));

    ASSERT_EQ_INT(0x11223344, beacon_data_int(&parser));
    ASSERT_EQ_INT(0x0102, beacon_data_short(&parser));
    ASSERT_EQ_INT(0, beacon_data_length(&parser));
}

static void test_beacon_data_extract_returns_length_prefixed_blob(void) {
    unsigned char raw[] = {
        0, 0, 0, 0,
        0x03, 0x00, 0x00, 0x00,
        'c', 'a', 't',
        0x01, 0x00, 0x00, 0x00
    };
    datap parser = {0};

    beacon_data_parse(&parser, (char*)raw, sizeof(raw));

    INT blob_size = 0;
    PCHAR blob = beacon_data_extract(&parser, &blob_size);

    ASSERT_EQ_INT(3, blob_size);
    ASSERT_MEM_EQ("cat", blob, 3);
    ASSERT_EQ_INT(4, parser.length);
}

static void test_beacon_format_printf_and_to_string(void) {
    formatp fmt = {0};
    beacon_format_alloc(&fmt, 64);

    beacon_format_printf(&fmt, "value=%d", 7);

    INT out_size = 0;
    PCHAR out = beacon_format_to_string(&fmt, &out_size);

    ASSERT_EQ_INT(7, out_size);
    ASSERT_STR_EQ("value=7", out);

    beacon_format_free(&fmt);
}

static void test_beacon_format_int_writes_big_endian_bytes(void) {
    formatp fmt = {0};
    beacon_format_alloc(&fmt, 16);

    beacon_format_int(&fmt, 0x11223344);

    ASSERT_EQ_INT(4, fmt.length);
    ASSERT_MEM_EQ("\x11\x22\x33\x44", fmt.original, 4);

    beacon_format_free(&fmt);
}

static void test_beacon_format_reset_restores_buffer(void) {
    formatp fmt = {0};
    beacon_format_alloc(&fmt, 8);
    beacon_format_append(&fmt, "abc", 3);

    beacon_format_reset(&fmt);

    for (int i = 0; i < fmt.size; i++) {
        ASSERT_EQ_INT(0, fmt.original[i]);
    }
    ASSERT_PTR_EQ(fmt.original, fmt.buffer);
    ASSERT_EQ_INT(fmt.size, fmt.length);

    beacon_format_free(&fmt);
}

static void test_beacon_format_free_clears_state(void) {
    formatp fmt = {0};
    beacon_format_alloc(&fmt, 8);
    beacon_format_free(&fmt);

    ASSERT_PTR_EQ(NULL, fmt.original);
    ASSERT_PTR_EQ(NULL, fmt.buffer);
    ASSERT_EQ_INT(0, fmt.length);
    ASSERT_EQ_INT(0, fmt.size);
}

static void test_beacon_format_append_moves_pointer(void) {
    formatp fmt = {0};
    beacon_format_alloc(&fmt, 16);
    beacon_format_append(&fmt, "dog", 3);

    ASSERT_EQ_INT(3, fmt.length);
    ASSERT_MEM_EQ("dog", fmt.original, 3);

    beacon_format_free(&fmt);
}

static void test_beacon_output_accumulates_printf_and_output(void) {
    windows_stub_reset();
    clear_beacon_output();

    beacon_printf(CALLBACK_OUTPUT, "hello %d", 5);
    const char suffix[] = "done";
    beacon_output(CALLBACK_OUTPUT, (PCHAR)suffix, sizeof(suffix) - 1);

    INT out_size = 0;
    PCHAR out = beacon_get_output_data(&out_size);

    ASSERT_EQ_INT(11, out_size);
    ASSERT_MEM_EQ("hello 5done", out, 11);

    mcfree(out);
}

static void test_to_wide_char_converts_ascii(void) {
    WCHAR buffer[8];
    BOOL success = to_wide_char("hi", buffer, sizeof(buffer));
    ASSERT_TRUE(success);
    ASSERT_EQ_INT('h', buffer[0]);
    ASSERT_EQ_INT('i', buffer[1]);
    ASSERT_EQ_INT('\0', buffer[2]);
}

static void test_beacon_use_token_records_token(void) {
    windows_stub_reset();
    HANDLE token = (HANDLE)0x4242;
    ASSERT_TRUE(beacon_use_token(token));
    windows_stub_observations obs = windows_stub_last_observations();
    ASSERT_PTR_EQ(token, obs.last_thread_token);
}

static void test_beacon_is_admin_follows_membership_result(void) {
    windows_stub_reset();
    windows_stub_set_check_token_membership_result(TRUE);
    ASSERT_TRUE(beacon_is_admin());
    windows_stub_set_check_token_membership_result(FALSE);
    ASSERT_FALSE(beacon_is_admin());
}

static void test_beacon_get_spawn_to_builds_expected_paths(void) {
    static const char expected_x86[] = "C:\\Windows\\" X86PATH "\\" DEFAULTPROCESSNAME;
    static const char expected_x64[] = "C:\\Windows\\" X64PATH "\\" DEFAULTPROCESSNAME;

    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    beacon_get_spawn_to(TRUE, buffer, (INT)sizeof(buffer));
    ASSERT_STR_EQ(expected_x86, buffer);

    memset(buffer, 0, sizeof(buffer));
    beacon_get_spawn_to(FALSE, buffer, (INT)sizeof(buffer));
    ASSERT_STR_EQ(expected_x64, buffer);
}

static void test_beacon_get_spawn_to_respects_length_limit(void) {
    char buffer[4] = {'z', 0, 0, 0};
    beacon_get_spawn_to(TRUE, buffer, 1);
    ASSERT_EQ_INT('z', buffer[0]);
}

static void test_beacon_cleanup_process_closes_handles(void) {
    windows_stub_reset();
    PROCESS_INFORMATION info = {0};
    info.hThread = (HANDLE)0x100;
    info.hProcess = (HANDLE)0x200;
    beacon_cleanup_process(&info);

    windows_stub_observations obs = windows_stub_last_observations();
    ASSERT_EQ_SIZE(2, obs.closed_handle_count);
    ASSERT_PTR_EQ((HANDLE)0x100, obs.closed_handles[0]);
    ASSERT_PTR_EQ((HANDLE)0x200, obs.closed_handles[1]);
}

void register_beacon_function_tests(test_suite* suite) {
    test_suite_add(suite, "beacon_data_parse_sets_initial_state", test_beacon_data_parse_sets_initial_state);
    test_suite_add(suite, "beacon_data_int_and_short_consume_stream", test_beacon_data_int_and_short_consume_stream);
    test_suite_add(suite, "beacon_data_extract_returns_length_prefixed_blob", test_beacon_data_extract_returns_length_prefixed_blob);
    test_suite_add(suite, "beacon_format_printf_and_to_string", test_beacon_format_printf_and_to_string);
    test_suite_add(suite, "beacon_format_int_writes_big_endian_bytes", test_beacon_format_int_writes_big_endian_bytes);
    test_suite_add(suite, "beacon_output_accumulates_printf_and_output", test_beacon_output_accumulates_printf_and_output);
    test_suite_add(suite, "beacon_format_reset_restores_buffer", test_beacon_format_reset_restores_buffer);
    test_suite_add(suite, "beacon_format_free_clears_state", test_beacon_format_free_clears_state);
    test_suite_add(suite, "beacon_format_append_moves_pointer", test_beacon_format_append_moves_pointer);
    test_suite_add(suite, "to_wide_char_converts_ascii", test_to_wide_char_converts_ascii);
    test_suite_add(suite, "beacon_use_token_records_token", test_beacon_use_token_records_token);
    test_suite_add(suite, "beacon_is_admin_follows_membership_result", test_beacon_is_admin_follows_membership_result);
    test_suite_add(suite, "beacon_get_spawn_to_builds_expected_paths", test_beacon_get_spawn_to_builds_expected_paths);
    test_suite_add(suite, "beacon_get_spawn_to_respects_length_limit", test_beacon_get_spawn_to_respects_length_limit);
    test_suite_add(suite, "beacon_cleanup_process_closes_handles", test_beacon_cleanup_process_closes_handles);
}
