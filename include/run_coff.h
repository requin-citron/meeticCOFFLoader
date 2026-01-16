#pragma once
#ifndef RUN_COFF_H
#define RUN_COFF_H

#include <windows.h>
#include "coff_loader.h"
#include "debug.h"
#include "utils.h"

#define ENTRY_POINT_NAME "go"

/* AMD64 Specific types */
#define IMAGE_REL_AMD64_ABSOLUTE    0x0000
#define IMAGE_REL_AMD64_ADDR64      0x0001
#define IMAGE_REL_AMD64_ADDR32      0x0002
#define IMAGE_REL_AMD64_ADDR32NB    0x0003
/* Most common from the looks of it, just 32-bit relative address from the byte following the relocation */
#define IMAGE_REL_AMD64_REL32       0x0004
/* Second most common, 32-bit address without an image base. Not sure what that means... */
#define IMAGE_REL_AMD64_REL32_1     0x0005
#define IMAGE_REL_AMD64_REL32_2     0x0006
#define IMAGE_REL_AMD64_REL32_3     0x0007
#define IMAGE_REL_AMD64_REL32_4     0x0008
#define IMAGE_REL_AMD64_REL32_5     0x0009
#define IMAGE_REL_AMD64_SECTION     0x000A
#define IMAGE_REL_AMD64_SECREL      0x000B
#define IMAGE_REL_AMD64_SECREL7     0x000C
#define IMAGE_REL_AMD64_TOKEN       0x000D
#define IMAGE_REL_AMD64_SREL32      0x000E
#define IMAGE_REL_AMD64_PAIR        0x000F
#define IMAGE_REL_AMD64_SSPAN32     0x0010

__attribute__((__annotate__(("flatten,boguscfg,substitution,indirectcall"))))
int run_coff(PBYTE coff_data, DWORD coff_size, PBYTE param_data, DWORD param_size);

#endif // RUN_COFF_H