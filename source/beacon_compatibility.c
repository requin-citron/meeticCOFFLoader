/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */
#include "beacon_compatibility.h"

PCHAR beacon_compatibility_output = NULL;
INT   beacon_compatibility_size   = 0;
INT   beacon_compatibility_offset = 0;

PBYTE beacon_compatibility_filecontent = NULL;
SIZE_T beacon_compatibility_filesize   = 0;
SIZE_T beacon_compatibility_allocsize  = 0;

 /* Data Parsing */
PCHAR internal_functions[INTERNAL_FUNCTIONS_COUNT][2] = {
    {(PCHAR)"BeaconDataParse",      (PCHAR)beacon_data_parse},
    {(PCHAR)"BeaconDataInt",        (PCHAR)beacon_data_int},
    {(PCHAR)"BeaconDataShort",      (PCHAR)beacon_data_short},
    {(PCHAR)"BeaconDataLength",     (PCHAR)beacon_data_length},
    {(PCHAR)"BeaconDataExtract",    (PCHAR)beacon_data_extract},
    {(PCHAR)"BeaconFormatAlloc",    (PCHAR)beacon_format_alloc},
    {(PCHAR)"BeaconFormatReset",    (PCHAR)beacon_format_reset},
    {(PCHAR)"BeaconFormatFree",     (PCHAR)beacon_format_free},
    {(PCHAR)"BeaconFormatAppend",   (PCHAR)beacon_format_append},
    {(PCHAR)"BeaconFormatPrintf",   (PCHAR)beacon_format_printf},
    {(PCHAR)"BeaconFormatToString", (PCHAR)beacon_format_to_string},
    {(PCHAR)"BeaconFormatInt",      (PCHAR)beacon_format_int},
    {(PCHAR)"BeaconPrintf",         (PCHAR)beacon_printf},
    {(PCHAR)"BeaconOutput",         (PCHAR)beacon_output},
    {(PCHAR)"BeaconUseToken",       (PCHAR)beacon_use_token},
    {(PCHAR)"BeaconRevertToken",    (PCHAR)beacon_revert_token},
    {(PCHAR)"BeaconIsAdmin",        (PCHAR)beacon_is_admin},
    {(PCHAR)"BeaconGetSpawnTo",     (PCHAR)beacon_get_spawn_to},
    {(PCHAR)"BeaconSpawnTemporaryProcess",  (PCHAR)beacon_spawn_temporary_process},
    {(PCHAR)"BeaconInjectProcess",  (PCHAR)beacon_inject_process},
    {(PCHAR)"BeaconInjectTemporaryProcess", (PCHAR)beacon_inject_temporary_process},
    {(PCHAR)"BeaconCleanupProcess", (PCHAR)beacon_cleanup_process},
    {(PCHAR)"toWideChar",           (PCHAR)to_wide_char},
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

BOOL to_wide_char(PCHAR src, PWCHAR dst, INT max) {
    if (max < sizeof(WCHAR))
        return FALSE;
    return MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, src, -1, dst, max / sizeof(WCHAR));
}
