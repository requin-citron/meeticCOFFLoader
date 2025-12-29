#include "utils.h"

//read file to buffer
PBYTE ReadFileToBuffer(PCHAR filePath, PDWORD fileSize) {
    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    DWORD size = GetFileSize(hFile, NULL);
    if (size == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        return NULL;
    }

    PBYTE buffer = (PBYTE)mcalloc(size);
    if (!buffer) {
        CloseHandle(hFile);
        return NULL;
    }

    DWORD bytesRead;
    if (!ReadFile(hFile, buffer, size, &bytesRead, NULL) || bytesRead != size) {
        mcfree(buffer);
        CloseHandle(hFile);
        return NULL;
    }

    CloseHandle(hFile);
    *fileSize = size;
    return buffer;
}

PCHAR unhexlify(PCHAR value, PDWORD out_size) {

    if (!value || !out_size) {
        return NULL;
    }

    size_t len = strlen(value);
    if (len % 2 != 0) {
        return NULL; // Invalid hex string
    }

    size_t finalLen = len / 2;
    PCHAR result = (PCHAR)mcalloc(finalLen + 1);
    if (!result) {
        return NULL; // Memory allocation failed
    }

    for (size_t i = 0; i < finalLen; i++) {
        sscanf(&value[i * 2], "%2hhx", &result[i]);
    }

    result[finalLen] = '\0'; // Null-terminate the string
    *out_size = (DWORD)finalLen;
    return result;
}

