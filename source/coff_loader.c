#include "coff_loader.h"

BOOL coff_symbol_is_defined(PCOFF_SYM symbol) {
    return (symbol->section_number > 0);
}

BOOL coff_symbol_is_external(PCOFF_SYM symbol) {
    return symbol->storage_class == IMAGE_SYM_CLASS_EXTERNAL_DEF
        || symbol->storage_class == IMAGE_SYM_CLASS_EXTERNAL;
}

static BOOL starts_with(PCHAR string, PCHAR substring) {
    return CompareStringA(LOCALE_INVARIANT, 0, string, lstrlenA(substring), substring, lstrlenA(substring)) == CSTR_EQUAL;
}

/* Helper function to process a symbol string, determine what function and
 * library its from, and return the right function pointer. Will need to
 * implement in the loading of the beacon internal functions, or any other
 * internal functions you want to have available. */
PVOID process_symbol(PCHAR symbolstring) {
    PVOID functionaddress = NULL;
    CHAR localcopy[1024]  = { 0 };
    CHAR* locallib        = NULL;
    CHAR* localfunc       = NULL;
    INT32 tempcounter     = 0;
    HMODULE llHandle      = NULL;


    RtlMoveMemory(localcopy, symbolstring, lstrlenA(symbolstring) + 1);
    if (starts_with(symbolstring, PREPENDSYMBOLVALUE"Beacon") || starts_with(symbolstring, PREPENDSYMBOLVALUE"toWideChar") ||
        starts_with(symbolstring, PREPENDSYMBOLVALUE"GetProcAddress") || starts_with(symbolstring, PREPENDSYMBOLVALUE"LoadLibraryA") ||
        starts_with(symbolstring, PREPENDSYMBOLVALUE"GetModuleHandleA") || starts_with(symbolstring, PREPENDSYMBOLVALUE"FreeLibrary") ||
        starts_with(symbolstring, "__C_specific_handler")) {
        if(lstrcmpA(symbolstring, "__C_specific_handler") == 0)
        {
            localfunc = symbolstring;
            return internal_functions[29][1];
        }
        else
        {
            localfunc = symbolstring + lstrlenA(PREPENDSYMBOLVALUE);
        }
        _inf_coffloader("InternalFunction: %s", localfunc);
        /* TODO: Get internal symbol here and set to functionaddress, then
         * return the pointer to the internal function*/

        for (tempcounter = 0; tempcounter < 30; tempcounter++) {
            if (internal_functions[tempcounter][0] != NULL) {
                if (starts_with(localfunc, (char*)(internal_functions[tempcounter][0]))) {
                    functionaddress = (void*)internal_functions[tempcounter][1];
                    return functionaddress;
                }
            }
        }

    }
    else if (strncmp(symbolstring, PREPENDSYMBOLVALUE, lstrlenA(PREPENDSYMBOLVALUE)) == 0) {
        _inf_coffloader("Yep its an external symbol");
        locallib = localcopy + lstrlenA(PREPENDSYMBOLVALUE);

        PCHAR dollar1 = strchr(locallib, '$');
        *dollar1 = '\0';
        localfunc = dollar1 + 1;
        _inf_coffloader("Library  : %s", locallib);
        PCHAR at_sign = strchr(localfunc, '@');
        if (at_sign) {
            *at_sign = '\0';  //replace @ with null terminator
        }
        _inf_coffloader("Function : %s", localfunc);

        /* Resolve the symbols here, and set the functionpointervalue */
        llHandle = GetModuleHandleA(locallib);
        //Make sure to load the library if not already loaded
        if( llHandle == NULL ) {
            llHandle = LoadLibraryA(locallib);
        }
        _inf_coffloader("Handle      : 0x%p", llHandle);
        functionaddress = GetProcAddress(llHandle, localfunc);
        _inf_coffloader("ProcAddress : 0x%p", functionaddress);
    }
    return functionaddress;
}

PBYTE beacon_get_output_data(int *out_size) {
    if (out_size == NULL) {
        return NULL;
    }

    PBYTE outdata = (PBYTE)beacon_compatibility_output;
    *out_size     = beacon_compatibility_size;
    beacon_compatibility_output = NULL;
    beacon_compatibility_size   = 0;
    beacon_compatibility_offset = 0;

    return outdata;
}

#if _DEBUG_COFFLOADER==1

void print_coff_header(PCOFF_FILE_HEADER header) {
    _inf_coffloader("Machine                 : 0x%04X", header->machine);
    _inf_coffloader("Number of Sections      : %d", header->number_of_sections);
    _inf_coffloader("Time Date Stamp         : %lu", header->time_date_stamp);
    _inf_coffloader("Pointer to Symbol Table : 0x%08lX", header->pointer_to_symbol_table);
    _inf_coffloader("Number of Symbols       : %lu", header->number_of_symbols);
    _inf_coffloader("Size of Optional Header : %d", header->size_of_optional_header);
    _inf_coffloader("Characteristics         : 0x%04X", header->characteristics);
}

void print_coff_section(PCOFF_SECT section) {
    char name[9] = {0};
    memcpy(name, section->name, 8);
    _inf_coffloader("Section Name            : %s", name);
    _inf_coffloader("Virtual Size            : 0x%08lX", section->virtual_size);
    _inf_coffloader("Virtual Address         : 0x%08lX", section->virtual_address);
    _inf_coffloader("Size of Raw Data        : 0x%08lX", section->size_of_raw_data);
    _inf_coffloader("Pointer to Raw Data     : 0x%08lX", section->pointer_to_raw_data);
    _inf_coffloader("Pointer to Relocations  : 0x%08lX", section->pointer_to_relocations);
    _inf_coffloader("Pointer to Line Numbers : 0x%08lX", section->pointer_to_line_numbers);
    _inf_coffloader("Number of Relocations   : %d", section->number_of_relocations);
    _inf_coffloader("Number of Line Numbers  : %d", section->number_of_line_numbers);
    _inf_coffloader("Characteristics         : 0x%08lX", section->characteristics);
}   

#endif