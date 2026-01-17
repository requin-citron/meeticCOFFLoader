#pragma once
#ifndef CLI_UTILS_H
#define CLI_UTILS_H

#include <windows.h>
#include <stdio.h>

#define mcalloc(X)       (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (X))
#define mcfree(X)        HeapFree(GetProcessHeap(), 0, (X))
#define mcrealloc(X, Y)  (PBYTE)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (X), (Y))

PBYTE ReadFileToBuffer(PCHAR, PDWORD);
PCHAR unhexlify(PCHAR, PDWORD);

#endif // CLI_UTILS_H