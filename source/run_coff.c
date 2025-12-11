#include "run_coff.h"

int run_coff(PCHAR coff_data, DWORD coff_size, PCHAR param_data, DWORD param_size) {
    PCOFF_SECT coff_sect_ptr   = NULL;
    PCOFF_RELOC coff_reloc_ptr = NULL;
    SIZE_T relocation_count    = 0;
    PCHAR symbol_name          = NULL;
    
    PCOFF_FILE_HEADER coff_header = (PCOFF_FILE_HEADER)coff_data;
    
    // Byte index of the strtab/end of symtab
    SIZE_T coff_strtab_index      = coff_header->pointer_to_symbol_table + (coff_header->number_of_symbols * sizeof(COFF_SYM));
    SIZE_T coff_strtab_size       = *(PDWORD)(coff_data + coff_strtab_index);
    
    PCOFF_SYM coff_sym_ptr      = (PCOFF_SYM)(coff_data + coff_header->pointer_to_symbol_table);
    PBYTE* section_mapping        = NULL;
    PINT32 section_size           = NULL;
    PVOID* function_mapping       = NULL;
    PVOID funcptr_location        = NULL;
    INT32 function_mapping_count  = 0;
    
    /* Buffer to hold the symbol short name if the symbol has no trailing NULL byte */
    CHAR symbol_shortname_buffer[9] = {0};

    section_mapping = (PBYTE*)mcalloc(sizeof(PBYTE) * (coff_header->number_of_sections + 1));
    section_size    = (PINT32)mcalloc(sizeof(INT32) * (coff_header->number_of_sections + 1));

    print_coff_header(coff_header);
    _log_no_format("\n");

    /* Handle the allocation and copying of the sections we're going to use
     * for right now I'm just VirtualAlloc'ing memory, this can be changed to
     * other methods, but leaving that up to the person implementing it. */
    for (SIZE_T i = 0; i < coff_header->number_of_sections; i++) {
        coff_sect_ptr = (PCOFF_SECT)(coff_data + sizeof(COFF_FILE_HEADER) + (sizeof(COFF_SECT) * i));
        print_coff_section(coff_sect_ptr);

        relocation_count  += coff_sect_ptr->number_of_relocations;
        section_mapping[i] = (PBYTE)VirtualAlloc(NULL, coff_sect_ptr->size_of_raw_data, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
        section_size[i]    = coff_sect_ptr->size_of_raw_data;

        if(coff_sect_ptr->size_of_raw_data > 0) {
            printf("Copying 0x%lX bytes to section %llu\n", coff_sect_ptr->size_of_raw_data, i);
            RtlMoveMemory(section_mapping[i], coff_data + coff_sect_ptr->pointer_to_raw_data, coff_sect_ptr->size_of_raw_data);
        }else{
            RtlZeroMemory(section_mapping[i], coff_sect_ptr->virtual_size);
        }

        _log_no_format("\n");
    }

    /* Allocate and setup the GOT for functions, same here as above. */
    /* Actually allocate enough for worst case every relocation, may not be needed, but hey better safe than sorry */
    function_mapping = (PVOID*)VirtualAlloc(NULL, relocation_count*8, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);

    /* Start parsing the relocations, and *hopefully* handle them correctly. */
    for(WORD i = 0; i < coff_header->number_of_sections; i++) {
        coff_sect_ptr  = (PCOFF_SECT)(coff_data + sizeof(COFF_FILE_HEADER) + (sizeof(COFF_SECT) * i));
        coff_reloc_ptr = (PCOFF_RELOC)(coff_data + coff_sect_ptr->pointer_to_relocations);

        _inf("Processing %u relocations for section %s", coff_sect_ptr->number_of_relocations, coff_sect_ptr->name);
        for(WORD j = 0; j < coff_sect_ptr->number_of_relocations; j++) {
            _inf("Virtual Address : 0x%08lX", coff_reloc_ptr->virtual_address);
            _inf("Symbol Index    : 0x%02lX", coff_reloc_ptr->symbol_table_index);
            _inf("Type            : 0x%04X", coff_reloc_ptr->type);

            /* Check if the symbol name is a long symbol name */
            if(coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.value[0] == 0 ) {
                /* Long symbol name from the string table */
                symbol_name = ((char*)(coff_sym_ptr + coff_header->number_of_symbols))
                    + coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.value[1];
            } else {
                /* Short symbol name from the symbol table */
                /* If the short symbol name is 8 bytes in length, it is not NULL
                 * terminated. Copy it to a temporary buffer to add the NULL terminator. */
                if (coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.name[7] != '\0') {
                    RtlMoveMemory(
                        symbol_shortname_buffer,
                        &coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.name[0],
                        sizeof(coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.name)
                    );

                    symbol_name = symbol_shortname_buffer;
                } else {
                    symbol_name = &coff_sym_ptr[coff_reloc_ptr->symbol_table_index].first.name[0];
                }
            }
            
            _inf("sym_name_ptr   : %p", symbol_name);
            _inf("sym_name       : %s", symbol_name);
            _inf("section_number : %d", coff_sym_ptr[coff_reloc_ptr->symbol_table_index].section_number);
            
            /* Check if the target symbol is a local symbol or an external undefined symbol
             * and resolve it */
            if(coff_symbol_is_defined(&coff_sym_ptr[coff_reloc_ptr->symbol_table_index])) {
                /* Locally defined symbol. Find the mapped address. */
                funcptr_location = section_mapping[coff_sym_ptr[coff_reloc_ptr->symbol_table_index].section_number - 1];
                funcptr_location = (PVOID)((PCHAR)funcptr_location + coff_sym_ptr[coff_reloc_ptr->symbol_table_index].value);
            } else if(coff_symbol_is_external(&coff_sym_ptr[coff_reloc_ptr->symbol_table_index])) {
                /* Imported symbol. Resolve it and map it in */
                funcptr_location = process_symbol(symbol_name);

                /* Map the imported symbol address to the local import table */
                function_mapping[function_mapping_count] = funcptr_location;

                _inf("Resolved external symbol %s to address 0x%p", symbol_name, funcptr_location);

                /* Get the address of the imported symbol mapped in the local import 
                 * table for the relocation target */
                funcptr_location = &function_mapping[function_mapping_count];

                /* Increment the number of mapped imported functions */
                function_mapping_count += 1;
            } else {
                _wrn("Unhandled symbol type!");
            }
            
            coff_reloc_ptr += 1;

            _log_no_format("\n");
        }
    }

    return 0;
}