#include "beacon_compatibility.h"

/* Main output functions */

VOID beacon_printf(INT type, PCHAR fmt, ...) {
    if (fmt == NULL) {
        return;
    }

    INT length             = 0;
    PCHAR tempptr          = NULL;
    PCHAR formatted_buffer = NULL;
    va_list args;

    // Allouer un buffer temporaire pour le formatage (max 8KB)
    formatted_buffer = (PCHAR)mcalloc(BEACON_FORMAT_BUFFER_SIZE);
    if (formatted_buffer == NULL) {
        return;
    }

    // Formater la chaîne avec wvsprintfA (WinAPI)
    va_start(args, fmt);
    length = wvsprintfA(formatted_buffer, fmt, args);
    va_end(args);

    if (length <= 0) {
        mcfree(formatted_buffer);
        return;
    }

    // Réallouer le buffer de sortie
    if (beacon_compatibility_output == NULL) {
        tempptr = (PCHAR)mcalloc(length + 1);
    }
    else {
        tempptr = (PCHAR)mcrealloc(beacon_compatibility_output, beacon_compatibility_size + length + 1);
    }

    if (tempptr == NULL) {
        mcfree(formatted_buffer);
        return;
    }

    beacon_compatibility_output = tempptr;
    RtlZeroMemory(beacon_compatibility_output + beacon_compatibility_offset, length + 1);
    RtlMoveMemory(beacon_compatibility_output + beacon_compatibility_offset, formatted_buffer, length);
    
    beacon_compatibility_size   += length;
    beacon_compatibility_offset += length;

    mcfree(formatted_buffer);
    return;
}

VOID beacon_output(INT type, PCHAR data, SIZE_T len) {
    if (data == NULL) {
        return;
    }

    if(type == CALLBACK_FILE){
        if(beacon_compatibility_filecontent != NULL){
            VirtualFree(beacon_compatibility_filecontent, 0, MEM_RELEASE);
        }
        beacon_compatibility_filecontent = (PBYTE)VirtualAlloc(
                NULL, 
                BEACON_FILE_CHUNK_SIZE, 
                MEM_COMMIT | MEM_RESERVE, 
                PAGE_READWRITE
            );
        beacon_compatibility_allocsize = BEACON_FILE_CHUNK_SIZE;

        return;
    }else if(type == CALLBACK_FILE_WRITE){
        PBYTE writable_data  = (PBYTE)data + 4; // Skip the first 4 bytes cobalt strike uses
        SIZE_T writable_size = len - 4; // Adjust size accordingly
        
        if((beacon_compatibility_filesize + writable_size) > beacon_compatibility_allocsize){
            SIZE_T newsize = ( ( (beacon_compatibility_filesize + writable_size) / BEACON_FILE_CHUNK_SIZE ) + 1) * BEACON_FILE_CHUNK_SIZE;
            PBYTE temp = (PBYTE)VirtualAlloc(
                NULL, 
                newsize, 
                MEM_COMMIT | MEM_RESERVE, 
                PAGE_READWRITE
            );
            RtlMoveMemory(temp, beacon_compatibility_filecontent, beacon_compatibility_filesize);
            VirtualFree(beacon_compatibility_filecontent, 0, MEM_RELEASE);
            beacon_compatibility_filecontent = temp;
            beacon_compatibility_allocsize = newsize;

        }else{
            RtlMoveMemory(beacon_compatibility_filecontent + beacon_compatibility_filesize, writable_data, writable_size);
            beacon_compatibility_filesize += writable_size;
        }

        return;
    }else if(type == CALLBACK_FILE_CLOSE){
        return;
    }

    PCHAR tempptr = NULL;
    if(beacon_compatibility_output == NULL) {
        tempptr = (PCHAR)mcalloc(len+1);
    }
    else {
        tempptr = (PCHAR)mcrealloc(beacon_compatibility_output, beacon_compatibility_size + len + 1);
    }

    beacon_compatibility_output = tempptr;

    RtlZeroMemory(beacon_compatibility_output + beacon_compatibility_offset, len + 1);
    RtlMoveMemory(beacon_compatibility_output + beacon_compatibility_offset, data, len);

    beacon_compatibility_size   += len;
    beacon_compatibility_offset += len;
    return;
}