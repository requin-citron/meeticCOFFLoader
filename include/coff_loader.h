#pragma once
#ifndef COFF_LOADER_H
#define COFF_LOADER_H

#include <windows.h>
#include "debug.h"
#include "beacon_compatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These seem to be the same sizes across architectures, relocations are different though. Defined both sets of types. */
// https://github.com/trustedsec/COFFLoader/blob/main/COFFLoader.h

/* AMD64  should always be here */
#define MACHINETYPE_AMD64 0x8664
/* AMD64 */
#define PREPENDSYMBOLVALUE "__imp_"

/* sizeof 20 */
typedef struct {
    WORD  machine;
    WORD  number_of_sections;
    DWORD time_date_stamp;
    DWORD pointer_to_symbol_table;
    DWORD number_of_symbols;
    WORD  size_of_optional_header;
    WORD  characteristics;
} __attribute__((packed)) COFF_FILE_HEADER, *PCOFF_FILE_HEADER;

/* Size of 40 */
typedef struct {
    char  name[8];
    DWORD virtual_size;
    DWORD virtual_address;
    DWORD size_of_raw_data;
    DWORD pointer_to_raw_data;
    DWORD pointer_to_relocations;
    DWORD pointer_to_line_numbers;
    WORD  number_of_relocations;
    WORD  number_of_line_numbers;
    DWORD characteristics;
} __attribute__((packed)) COFF_SECT, *PCOFF_SECT;

typedef struct {
    DWORD virtual_address;
    DWORD symbol_table_index;
    WORD  type;
} __attribute__((packed)) COFF_RELOC, *PCOFF_RELOC;

typedef struct coff_sym {
    union {
        char name[8];
        DWORD value[2];
    } first;
    DWORD value;
    WORD  section_number;
    WORD  type;
    BYTE  storage_class;
    BYTE  number_of_aux_symbols;
} __attribute__((packed)) COFF_SYM, *PCOFF_SYM;

BOOL coff_symbol_is_external(PCOFF_SYM);
BOOL coff_symbol_is_defined(PCOFF_SYM);

__attribute__((__annotate__(("flatten,substitution,indirectcall"))))
PVOID process_symbol(PCHAR);

VOID beacon_get_file_data(PBYTE *, SIZE_T *);
VOID beacon_free_file_data();

#if _DEBUG_COFFLOADER==1

void print_coff_header(PCOFF_FILE_HEADER);
void print_coff_section(PCOFF_SECT);

#endif

#ifdef __cplusplus
}
#endif

#endif 