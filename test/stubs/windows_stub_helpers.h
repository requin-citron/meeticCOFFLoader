#ifndef TEST_WINDOWS_STUB_HELPERS_H
#define TEST_WINDOWS_STUB_HELPERS_H

#include "windows.h"
#include <stddef.h>

typedef struct {
    char load_library[64];
    char get_module_handle[64];
    char proc_lookup[64];
    HANDLE last_thread_token;
    size_t closed_handle_count;
    HANDLE closed_handles[4];
} windows_stub_observations;

void windows_stub_reset(void);
void windows_stub_set_module_handle_result(HMODULE module);
void windows_stub_set_load_library_result(HMODULE module);
void windows_stub_set_next_proc_address(FARPROC proc);
void windows_stub_set_check_token_membership_result(BOOL is_member);
windows_stub_observations windows_stub_last_observations(void);

#endif /* TEST_WINDOWS_STUB_HELPERS_H */
