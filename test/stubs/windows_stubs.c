#include "windows_stub_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static HANDLE g_heap_handle = (HANDLE)0x1;
static HMODULE g_module_handle_result = NULL;
static HMODULE g_load_library_result  = (HMODULE)0x1;
static FARPROC g_next_proc_address    = NULL;
static BOOL g_check_token_member      = TRUE;
static windows_stub_observations g_observations;

static void remember(char target[64], const char* value) {
    if (value == NULL) {
        target[0] = '\0';
        return;
    }

    strncpy(target, value, 63);
    target[63] = '\0';
}

void windows_stub_reset(void) {
    g_module_handle_result = NULL;
    g_load_library_result  = (HMODULE)0x1;
    g_next_proc_address    = (FARPROC)0x1;
    g_check_token_member   = TRUE;
    memset(&g_observations, 0, sizeof(g_observations));
}

void windows_stub_set_module_handle_result(HMODULE module) {
    g_module_handle_result = module;
}

void windows_stub_set_load_library_result(HMODULE module) {
    g_load_library_result = module;
}

void windows_stub_set_next_proc_address(FARPROC proc) {
    g_next_proc_address = proc;
}

void windows_stub_set_check_token_membership_result(BOOL is_member) {
    g_check_token_member = is_member;
}

windows_stub_observations windows_stub_last_observations(void) {
    return g_observations;
}

int lstrlenA(LPCSTR string) {
    if (string == NULL) {
        return 0;
    }
    return (int)strlen(string);
}

int lstrcmpA(LPCSTR string1, LPCSTR string2) {
    if (string1 == NULL && string2 == NULL) {
        return 0;
    }
    if (string1 == NULL) {
        return -1;
    }
    if (string2 == NULL) {
        return 1;
    }
    return strcmp(string1, string2);
}

int CompareStringA(int locale, DWORD flags, LPCSTR string1, int count1, LPCSTR string2, int count2) {
    (void)locale;
    (void)flags;

    if (string1 == NULL || string2 == NULL) {
        return 0;
    }

    if (count1 < 0) {
        count1 = lstrlenA(string1);
    }
    if (count2 < 0) {
        count2 = lstrlenA(string2);
    }

    size_t compare_length = (size_t)((count1 < count2) ? count1 : count2);
    if (strncmp(string1, string2, compare_length) == 0 && count1 == count2) {
        return CSTR_EQUAL;
    }

    return 0;
}

int wvsprintfA(LPSTR buffer, LPCSTR format, va_list args) {
    if (buffer == NULL || format == NULL) {
        return -1;
    }
    return vsnprintf(buffer, 0x7fffffff, format, args);
}

int MultiByteToWideChar(UINT codePage, DWORD flags, LPCSTR src, int srcLength, PWCHAR dest, int destLength) {
    (void)codePage;
    (void)flags;
    if (src == NULL || dest == NULL || destLength <= 0) {
        return 0;
    }

    int count = 0;
    while ((srcLength < 0 || count < srcLength) && src[count] != '\0' && count < destLength - 1) {
        dest[count] = (unsigned char)src[count];
        count++;
    }
    dest[count] = L'\0';
    return count;
}

HANDLE GetProcessHeap(void) {
    return g_heap_handle;
}

PVOID HeapAlloc(HANDLE heap, DWORD flags, SIZE_T size) {
    (void)heap;
    void* memory = NULL;
    if (flags & HEAP_ZERO_MEMORY) {
        memory = calloc(1, size);
    } else {
        memory = malloc(size);
    }
    return memory;
}

PVOID HeapReAlloc(HANDLE heap, DWORD flags, PVOID memory, SIZE_T size) {
    (void)heap;
    void* result = realloc(memory, size);
    if (result && (flags & HEAP_ZERO_MEMORY)) {
        /* Attempt to zero the new region. Without tracking the original size this
         * is best-effort and assumes the caller will zero as needed. */
    }
    return result;
}

BOOL HeapFree(HANDLE heap, DWORD flags, PVOID memory) {
    (void)heap;
    (void)flags;
    free(memory);
    return TRUE;
}

HMODULE GetModuleHandleA(LPCSTR moduleName) {
    remember(g_observations.get_module_handle, moduleName);
    return g_module_handle_result;
}

HMODULE LoadLibraryA(LPCSTR moduleName) {
    remember(g_observations.load_library, moduleName);
    return g_load_library_result;
}

FARPROC GetProcAddress(HMODULE module, LPCSTR procedureName) {
    (void)module;
    remember(g_observations.proc_lookup, procedureName);
    return g_next_proc_address;
}

BOOL FreeLibrary(HMODULE module) {
    (void)module;
    return TRUE;
}

BOOL CreateProcessA(LPCSTR applicationName,
                    LPSTR commandLine,
                    PVOID processAttributes,
                    PVOID threadAttributes,
                    BOOL inheritHandles,
                    DWORD creationFlags,
                    PVOID environment,
                    LPCSTR currentDirectory,
                    STARTUPINFO* startupInfo,
                    PROCESS_INFORMATION* processInformation) {
    (void)applicationName;
    (void)commandLine;
    (void)processAttributes;
    (void)threadAttributes;
    (void)inheritHandles;
    (void)creationFlags;
    (void)environment;
    (void)currentDirectory;

    if (startupInfo == NULL || processInformation == NULL) {
        return FALSE;
    }

    startupInfo->cb = sizeof(*startupInfo);
    processInformation->hProcess = (HANDLE)0x1234;
    processInformation->hThread  = (HANDLE)0x5678;
    processInformation->dwProcessId = 1;
    processInformation->dwThreadId  = 1;
    return TRUE;
}

BOOL CloseHandle(HANDLE object) {
    if (g_observations.closed_handle_count < (sizeof(g_observations.closed_handles) / sizeof(g_observations.closed_handles[0]))) {
        g_observations.closed_handles[g_observations.closed_handle_count++] = object;
    }
    return TRUE;
}

BOOL SetThreadToken(PHANDLE thread, HANDLE token) {
    (void)thread;
    g_observations.last_thread_token = token;
    return TRUE;
}

BOOL RevertToSelf(void) {
    return TRUE;
}

BOOL AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY authority,
                              BYTE subAuthorityCount,
                              DWORD subAuthority0,
                              DWORD subAuthority1,
                              DWORD subAuthority2,
                              DWORD subAuthority3,
                              DWORD subAuthority4,
                              DWORD subAuthority5,
                              DWORD subAuthority6,
                              DWORD subAuthority7,
                              PSID* sid) {
    (void)authority;
    (void)subAuthorityCount;
    (void)subAuthority0;
    (void)subAuthority1;
    (void)subAuthority2;
    (void)subAuthority3;
    (void)subAuthority4;
    (void)subAuthority5;
    (void)subAuthority6;
    (void)subAuthority7;

    if (sid == NULL) {
        return FALSE;
    }

    *sid = calloc(1, 8);
    return *sid != NULL;
}

BOOL CheckTokenMembership(HANDLE tokenHandle, PSID sidToCheck, PBOOL isMember) {
    (void)tokenHandle;
    (void)sidToCheck;
    if (isMember == NULL) {
        return FALSE;
    }
    *isMember = g_check_token_member;
    return TRUE;
}

void FreeSid(PSID sid) {
    free(sid);
}

void __C_specific_handler(void) {
}
