#include "run_coff.h"

void handle_relocation_type(PBYTE* section_mapping, SIZE_T index, PCOFF_SYM coff_sym_ptr, PCOFF_RELOC coff_reloc_ptr, PVOID funcptr){
    UINT64 longoffset_value = 0;
    UINT32 offset_value     = 0;

    PVOID destination      = (PVOID)(section_mapping[index] + coff_reloc_ptr->virtual_address);

    switch (coff_reloc_ptr->type) {
        /* Type == 1 relocation is the 64-bit VA of the relocation target */
        case IMAGE_REL_AMD64_ADDR64:
            RtlMoveMemory(&longoffset_value, destination, sizeof(UINT64));
            _inf("Readin offsetValue : 0x%llX", longoffset_value);
            longoffset_value += (UINT64)funcptr;
            _inf("Modified offsetValue : 0x%llX Base Address: %p", longoffset_value, funcptr);
            RtlMoveMemory(destination, &longoffset_value, sizeof(UINT64));
            
            break;
        /* This is Type == 3 relocation code */
        case IMAGE_REL_AMD64_ADDR32NB:
            RtlMoveMemory(&offset_value, destination, sizeof(UINT32));
            _inf("Readin OffsetValue : 0x%0X", offset_value);
            if (((char*)(section_mapping[coff_sym_ptr[coff_reloc_ptr->symbol_table_index].section_number - 1] + offset_value) - (char*)(destination + 4)) > 0xffffffff) {
                _inf("Relocations > 4 gigs away, exiting");
            }
            offset_value = ((char*)(section_mapping[coff_sym_ptr[coff_reloc_ptr->symbol_table_index].section_number - 1] + offset_value) - (char*)(destination + 4));
            offset_value += coff_sym_ptr[coff_reloc_ptr->symbol_table_index].value;
            _inf("Setting 0x%p to OffsetValue: 0x%X", destination, offset_value);
            RtlMoveMemory(destination, &offset_value, sizeof(UINT32));
        
            break;
        /* This is Type == 4 relocation code, this is either a relocation to a global or imported symbol */
        case IMAGE_REL_AMD64_REL32:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += ((size_t)funcptr - ((size_t)section_mapping[index] + coff_reloc_ptr->virtual_address + 4));
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));

            break;
        case IMAGE_REL_AMD64_REL32_1:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 1)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += (size_t)funcptr - ((size_t)section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 1);
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));

            break;
        case IMAGE_REL_AMD64_REL32_2:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 2)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += (size_t)funcptr - ((size_t)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 2));
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));
            
            break;
        case IMAGE_REL_AMD64_REL32_3:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 3)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += (size_t)funcptr - ((size_t)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 3));
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));

            break;
        case IMAGE_REL_AMD64_REL32_4:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 4)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += (size_t)funcptr - ((size_t)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 4));
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));

            break;
        case IMAGE_REL_AMD64_REL32_5:
            RtlMoveMemory(&offset_value, section_mapping[index] + coff_reloc_ptr->virtual_address, sizeof(UINT32));
            _inf("Readin offset value: 0x%X", offset_value);
            if (llabs((long long)funcptr - (long long)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 5)) > UINT_MAX) {
                _inf("Relocations > 4 gigs away, exiting");
                break;
            }
            offset_value += (size_t)funcptr - ((size_t)(section_mapping[index] + coff_reloc_ptr->virtual_address + 4 + 5));
            _inf("Setting 0x%p to relative address: 0x%X", section_mapping[index] + coff_reloc_ptr->virtual_address, offset_value);
            RtlMoveMemory(section_mapping[index] + coff_reloc_ptr->virtual_address, &offset_value, sizeof(UINT32));

            break;
        default:
            _inf("Relocation type 0x%04X not implemented yet!", coff_reloc_ptr->type);
            break;
    }

}

INT run_coff(PCHAR coff_data, DWORD coff_size, PCHAR param_data, DWORD param_size) {
    PCOFF_SECT coff_sect_ptr   = NULL;
    PCOFF_RELOC coff_reloc_ptr = NULL;
    PCHAR symbol_name          = NULL;
    SIZE_T relocation_count    = 0;
    
    VOID(*entrypoint)(PCHAR in, ULONG datalen);
    
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

        relocation_count += coff_sect_ptr->number_of_relocations;

        SIZE_T allocation_size = coff_sect_ptr->size_of_raw_data;
        if (coff_sect_ptr->virtual_size > allocation_size) {
            allocation_size = coff_sect_ptr->virtual_size;
        }

        if (allocation_size == 0) {
            allocation_size = coff_sect_ptr->size_of_raw_data;
        }

        section_mapping[i] = (PBYTE)VirtualAlloc(NULL, allocation_size, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
        section_size[i]    = (INT32)allocation_size;

        BOOL has_raw_data = coff_sect_ptr->pointer_to_raw_data != 0 && coff_sect_ptr->size_of_raw_data > 0;
        if(has_raw_data) {
            printf("Copying 0x%lX bytes to section %llu\n", coff_sect_ptr->size_of_raw_data, i);
            RtlMoveMemory(section_mapping[i], coff_data + coff_sect_ptr->pointer_to_raw_data, coff_sect_ptr->size_of_raw_data);
        }

        SIZE_T zero_offset = has_raw_data ? coff_sect_ptr->size_of_raw_data : 0;
        if (allocation_size > zero_offset) {
            /* BSS/uninitialized sections provide no raw data; ensure they start cleared */
            RtlZeroMemory(section_mapping[i] + zero_offset, allocation_size - zero_offset);
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

            handle_relocation_type(
                section_mapping,
                i,
                coff_sym_ptr,
                coff_reloc_ptr,
                funcptr_location
            );
            
            coff_reloc_ptr += 1;

            _log_no_format("\n");
        }
    }

    for(WORD i = 0; i < coff_header->number_of_sections; i++) {
        if(CompareStringA(
            LOCALE_INVARIANT,
            0,
            coff_sym_ptr[i].first.name,
            lstrlenA(ENTRY_POINT_NAME),
            ENTRY_POINT_NAME,
            lstrlenA(ENTRY_POINT_NAME)
        ) == CSTR_EQUAL) {
            _inf("Found entrypoint symbol %s in section %d at offset 0x%lX", ENTRY_POINT_NAME, coff_sym_ptr[i].section_number, coff_sym_ptr[i].value);
            entrypoint = (VOID(*)(PCHAR, ULONG))(section_mapping[coff_sym_ptr[i].section_number - 1] + coff_sym_ptr[i].value);

            entrypoint((PCHAR)param_data, param_size);
        }
    }

    _inf("back from entrypoint");

    // cleanup

    if(section_mapping) {
        for (SIZE_T i = 0; i < coff_header->number_of_sections; i++) {
            if(section_mapping[i]) {
                VirtualFree(section_mapping[i], 0, MEM_RELEASE);
            }
        }
        mcfree(section_mapping);
    }

    if(section_size) {
        mcfree(section_size);
    }

    if(function_mapping) {
        VirtualFree(function_mapping, 0, MEM_RELEASE);
    }

    _inf("Exiting run_coff");
    
    return 0;
}
