/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */
#ifdef _WIN32
#include "beacon_compatibility.h"

#define DEFAULTPROCESSNAME "rundll32.exe"
#ifdef _WIN64
#define X86PATH "SysWOW64"
#define X64PATH "System32"
#else
#define X86PATH "System32"
#define X64PATH "sysnative"
#endif

PCHAR beacon_compatibility_output = NULL;
INT   beacon_compatibility_size   = 0;
INT   beacon_compatibility_offset = 0;

 /* Data Parsing */
PCHAR internal_functions[30][2] = {
    {(PCHAR)"BeaconDataParse",      (PCHAR)BeaconDataParse},
    {(PCHAR)"BeaconDataInt",        (PCHAR)BeaconDataInt},
    {(PCHAR)"BeaconDataShort",      (PCHAR)BeaconDataShort},
    {(PCHAR)"BeaconDataLength",     (PCHAR)BeaconDataLength},
    {(PCHAR)"BeaconDataExtract",    (PCHAR)BeaconDataExtract},
    {(PCHAR)"BeaconFormatAlloc",    (PCHAR)BeaconFormatAlloc},
    {(PCHAR)"BeaconFormatReset",    (PCHAR)BeaconFormatReset},
    {(PCHAR)"BeaconFormatFree",     (PCHAR)BeaconFormatFree},
    {(PCHAR)"BeaconFormatAppend",   (PCHAR)BeaconFormatAppend},
    {(PCHAR)"BeaconFormatPrintf",   (PCHAR)BeaconFormatPrintf},
    {(PCHAR)"BeaconFormatToString", (PCHAR)BeaconFormatToString},
    {(PCHAR)"BeaconFormatInt",      (PCHAR)BeaconFormatInt},
    {(PCHAR)"BeaconPrintf",         (PCHAR)BeaconPrintf},
    {(PCHAR)"BeaconOutput",         (PCHAR)BeaconOutput},
    {(PCHAR)"BeaconUseToken",       (PCHAR)BeaconUseToken},
    {(PCHAR)"BeaconRevertToken",    (PCHAR)BeaconRevertToken},
    {(PCHAR)"BeaconIsAdmin",        (PCHAR)BeaconIsAdmin},
    {(PCHAR)"BeaconGetSpawnTo",     (PCHAR)BeaconGetSpawnTo},
    {(PCHAR)"BeaconSpawnTemporaryProcess", (PCHAR)BeaconSpawnTemporaryProcess},
    {(PCHAR)"BeaconInjectProcess",  (PCHAR)BeaconInjectProcess},
    {(PCHAR)"BeaconInjectTemporaryProcess", (PCHAR)BeaconInjectTemporaryProcess},
    {(PCHAR)"BeaconCleanupProcess", (PCHAR)BeaconCleanupProcess},
    {(PCHAR)"toWideChar",           (PCHAR)toWideChar},
    {(PCHAR)"LoadLibraryA",         (PCHAR)LoadLibraryA},
    {(PCHAR)"GetProcAddress",       (PCHAR)GetProcAddress},
    {(PCHAR)"GetModuleHandleA",     (PCHAR)GetModuleHandleA},
    {(PCHAR)"FreeLibrary",          (PCHAR)FreeLibrary},
    {(PCHAR)"__C_specific_handler", (PCHAR)__C_specific_handler}
};

UINT32 swap_endianess(UINT32 indata) {
    UINT32 testint = 0xaabbccdd;
    UINT32 outint = indata;
    if (((PBYTE)&testint)[0] == 0xdd) {
        ((PCHAR)&outint)[0] = ((PCHAR)&indata)[3];
        ((PCHAR)&outint)[1] = ((PCHAR)&indata)[2];
        ((PCHAR)&outint)[2] = ((PCHAR)&indata)[1];
        ((PCHAR)&outint)[3] = ((PCHAR)&indata)[0];
    }
    return outint;
}

VOID BeaconDataParse(datap* parser, PCHAR buffer, INT size) {
    if (parser == NULL || buffer == NULL) {
        return;
    }

    parser->original = buffer;
    parser->buffer = buffer;
    parser->length = size - 4;
    parser->size = size - 4;
    parser->buffer += 4;
    return;
}

INT BeaconDataInt(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    INT32 fourbyteint = 0;
    if (parser->length < 4) {
        return 0;
    }
    memcpy(&fourbyteint, parser->buffer, 4);
    parser->buffer += 4;
    parser->length -= 4;
    return (INT)fourbyteint;
}

SHORT BeaconDataShort(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    INT16 retvalue = 0;
    if (parser->length < 2) {
        return 0;
    }
    memcpy(&retvalue, parser->buffer, 2);
    parser->buffer += 2;
    parser->length -= 2;
    return (SHORT)retvalue;
}

INT BeaconDataLength(datap* parser) {
    if (parser == NULL) {
        return 0;
    }

    return parser->length;
}

PCHAR BeaconDataExtract(datap* parser, PINT size) {
    if (parser == NULL) {
        return NULL;
    }

    UINT32 length = 0;
    PCHAR outdata = NULL;
    /*Length prefixed binary blob, going to assume uint32_t for this.*/
    if (parser->length < 4) {
        return NULL;
    }
    memcpy(&length, parser->buffer, 4);
    parser->buffer += 4;

    outdata = parser->buffer;
    if (outdata == NULL) {
        return NULL;
    }
    parser->length -= 4;
    parser->length -= length;
    parser->buffer += length;
    if (size != NULL && outdata != NULL) {
        *size = length;
    }
    return outdata;
}

/* format API */

VOID BeaconFormatAlloc(formatp* format, INT maxsz) {
    if (format == NULL) {
        return;
    }

    format->original = mcalloc(maxsz);
    format->buffer = format->original;
    format->length = 0;
    format->size = maxsz;
    return;
}

VOID BeaconFormatReset(formatp* format) {
    if (format == NULL) {
        return;
    }

    memset(format->original, 0, format->size);
    format->buffer = format->original;
    format->length = format->size;
    return;
}

VOID BeaconFormatFree(formatp* format) {
    if (format == NULL) {
        return;
    }
    if (format->original) {
        mcfree(format->original);
        format->original = NULL;
    }
    format->buffer = NULL;
    format->length = 0;
    format->size = 0;
    return;
}

VOID BeaconFormatAppend(formatp* format, PCHAR text, INT len) {
    if (format == NULL || text == NULL) {
        return;
    }

    memcpy(format->buffer, text, len);
    format->buffer += len;
    format->length += len;
    return;
}

VOID BeaconFormatPrintf(formatp* format, PCHAR fmt, ...) {
    if (format == NULL || fmt == NULL) {
        return;
    }

    /*Take format string, and sprintf it into here*/
    va_list args;
    INT length = 0;

    va_start(args, fmt);
    length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (format->length + length + 1 > format->size) {
        return;
    }

    va_start(args, fmt);
    (VOID)vsnprintf(format->buffer, length + 1, fmt, args);
    va_end(args);
    format->length += length;
    format->buffer += length;
    return;
}


PCHAR BeaconFormatToString(formatp* format, PINT size) {
    if (format == NULL || size == NULL) {
        return NULL;
    }

    *size = format->length;
    return format->original;
}

VOID BeaconFormatInt(formatp* format, INT value) {
    if (format == NULL) {
        return;
    }

    UINT32 indata = value;
    UINT32 outdata = 0;
    if (format->length + 4 > format->size) {
        return;
    }
    outdata = swap_endianess(indata);
    memcpy(format->buffer, &outdata, 4);
    format->length += 4;
    format->buffer += 4;
    return;
}

/* Main output functions */

VOID BeaconPrintf(INT type, PCHAR fmt, ...) {
    if (fmt == NULL) {
        return;
    }

    /* Change to maintain internal buffer, and return after done running. */
    INT length = 0;
    PCHAR tempptr = NULL;
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    va_start(args, fmt);
    length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if(beacon_compatibility_output == NULL) {
        tempptr = (PCHAR)mcalloc(length + 1);
    }
    else {
        tempptr = (PCHAR)mcrealloc(beacon_compatibility_output, beacon_compatibility_size + length + 1);
    }   

    beacon_compatibility_output = tempptr;
    memset(beacon_compatibility_output + beacon_compatibility_offset, 0, length + 1);
    va_start(args, fmt);
    length = vsnprintf(beacon_compatibility_output + beacon_compatibility_offset, length +1, fmt, args);
    beacon_compatibility_size += length;
    beacon_compatibility_offset += length;
    va_end(args);
    return;
}

VOID BeaconOutput(INT type, PCHAR data, SIZE_T len) {
    if (data == NULL) {
        return;
    }

    PCHAR tempptr = NULL;
    if(beacon_compatibility_output == NULL) {
        tempptr = (PCHAR)mcalloc(len + 1);
    }
    else {
        tempptr = (PCHAR)mcrealloc(beacon_compatibility_output, beacon_compatibility_size + len + 1);
    }

    beacon_compatibility_output = tempptr;

    memset(beacon_compatibility_output + beacon_compatibility_offset, 0, len + 1);
    memcpy(beacon_compatibility_output + beacon_compatibility_offset, data, len);
    beacon_compatibility_size += len;
    beacon_compatibility_offset += len;
    return;
}

/* Token Functions */

BOOL BeaconUseToken(HANDLE token) {
    /* Probably needs to handle DuplicateTokenEx too */
    SetThreadToken(NULL, token);
    return TRUE;
}

VOID BeaconRevertToken(VOID) {
    if (!RevertToSelf()) 
        _inf("RevertToSelf Failed!\n");
    
    return;
}

BOOL BeaconIsAdmin(VOID) {
    /* Leaving this to be implemented by people needing it */
#ifdef DEBUG
    printf("BeaconIsAdmin Called\n");
#endif
    return FALSE;
}

/* Injection/spawning related stuffs
 *
 * These functions are basic place holders, and if implemented into something
 * real should be just calling internal functions for your tools. */
VOID BeaconGetSpawnTo(BOOL x86, PCHAR buffer, INT length) {
    PCHAR tempBufferPath = NULL;
    if (buffer == NULL) {
        return;
    }
    if (x86) {
        tempBufferPath = "C:\\Windows\\"X86PATH"\\"DEFAULTPROCESSNAME;
    }
    else {
        tempBufferPath = "C:\\Windows\\"X64PATH"\\"DEFAULTPROCESSNAME;
    }

    if ((INT)strlen(tempBufferPath) > length) {
        return;
    }
    memcpy(buffer, tempBufferPath, strlen(tempBufferPath));
    return;
}

BOOL BeaconSpawnTemporaryProcess(BOOL x86, BOOL ignoreToken, STARTUPINFO * sInfo, PROCESS_INFORMATION * pInfo) {
    BOOL bSuccess = FALSE;
    if (x86) {
        bSuccess = CreateProcessA(NULL, (PCHAR)"C:\\Windows\\"X86PATH"\\"DEFAULTPROCESSNAME, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, sInfo, pInfo);
    }
    else {
        bSuccess = CreateProcessA(NULL, (PCHAR)"C:\\Windows\\"X64PATH"\\"DEFAULTPROCESSNAME, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, sInfo, pInfo);
    }
    return bSuccess;
}

VOID BeaconInjectProcess(HANDLE hProc, int pid, PCHAR payload, int p_len, int p_offset, char * arg, int a_len) {
    /* Leaving this to be implemented by people needing/wanting it */
    return;
}

VOID BeaconInjectTemporaryProcess(PROCESS_INFORMATION* pInfo, PCHAR payload, int p_len, int p_offset, PCHAR arg, int a_len) {
    /* Leaving this to be implemented by people needing/wanting it */
    return;
}

VOID BeaconCleanupProcess(PROCESS_INFORMATION* pInfo) {
    if (pInfo != NULL) {
        (VOID)CloseHandle(pInfo->hThread);
        (VOID)CloseHandle(pInfo->hProcess);
    }
    return;
}

BOOL toWideChar(PCHAR src, PWCHAR dst, INT max) {
    if (max < sizeof(WCHAR))
        return FALSE;
    return MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, src, -1, dst, max / sizeof(WCHAR));
}

PCHAR BeaconGetOutputData(PINT outsize) {
    if (outsize == NULL) {
        return NULL;
    }

    PCHAR outdata = beacon_compatibility_output;
    *outsize      = beacon_compatibility_size;

    beacon_compatibility_output = NULL;
    beacon_compatibility_size   = 0;
    beacon_compatibility_offset = 0;
    
    return outdata;
}

#endif
