#include "beacon_compatibility.h"

/* Injection/spawning related stuffs
 *
 * These functions are basic place holders, and if implemented into something
 * real should be just calling internal functions for your tools. */
VOID beacon_get_spawn_to(BOOL x86, PCHAR buffer, INT length) {
    PCHAR tempBufferPath = NULL;
    if (buffer == NULL) {
        return;
    }
    if (x86) {
        tempBufferPath = "C:\\Windows\\" X86PATH "\\" DEFAULTPROCESSNAME;
    }
    else {
        tempBufferPath = "C:\\Windows\\" X64PATH "\\" DEFAULTPROCESSNAME;
    }

    if (lstrlenA(tempBufferPath) > length) {
        return;
    }
    RtlMoveMemory(buffer, tempBufferPath, lstrlenA(tempBufferPath));
    return;
}

BOOL beacon_spawn_temporary_process(BOOL x86, BOOL ignoreToken, STARTUPINFO * sInfo, PROCESS_INFORMATION * pInfo) {
    BOOL bSuccess = FALSE;
    if (x86) {
        bSuccess = CreateProcessA(NULL, (PCHAR)"C:\\Windows\\" X86PATH "\\" DEFAULTPROCESSNAME, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, sInfo, pInfo);
    }
    else {
        bSuccess = CreateProcessA(NULL, (PCHAR)"C:\\Windows\\" X64PATH "\\" DEFAULTPROCESSNAME, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, sInfo, pInfo);
    }
    return bSuccess;
}

VOID beacon_inject_process(HANDLE hProc, int pid, PCHAR payload, int p_len, int p_offset, char * arg, int a_len) {
    /* Leaving this to be implemented by people needing/wanting it */
    return;
}

VOID beacon_inject_temporary_process(PROCESS_INFORMATION* pInfo, PCHAR payload, int p_len, int p_offset, PCHAR arg, int a_len) {
    /* Leaving this to be implemented by people needing/wanting it */
    return;
}

VOID beacon_cleanup_process(PROCESS_INFORMATION* pInfo) {
    if (pInfo != NULL) {
        (VOID)CloseHandle(pInfo->hThread);
        (VOID)CloseHandle(pInfo->hProcess);
    }
    return;
}