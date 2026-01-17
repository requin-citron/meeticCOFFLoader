/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */
#ifndef BEACON_COMPATIBILITY_H_
#define BEACON_COMPATIBILITY_H_
#include <windows.h>
#include "debug.h"
#include "utils.h"

#define BEACON_FORMAT_BUFFER_SIZE 8192
#define BEACON_FILE_CHUNK_SIZE   (100 * 1024 * 1024 ) // 100MB

#define CALLBACK_OUTPUT      0x0
#define CALLBACK_OUTPUT_OEM  0x1e
#define CALLBACK_ERROR       0x0d
#define CALLBACK_OUTPUT_UTF8 0x20

#define CALLBACK_FILE       0x02
#define CALLBACK_FILE_WRITE 0x08
#define CALLBACK_FILE_CLOSE 0x09

#define DEFAULTPROCESSNAME "rundll32.exe"
#ifdef _WIN64
#define X86PATH "SysWOW64"
#define X64PATH "System32"
#else
#define X86PATH "System32"
#define X64PATH "sysnative"
#endif

#define INTERNAL_FUNCTIONS_COUNT 27

 /* Structures as is in beacon.h */
extern PCHAR internal_functions[INTERNAL_FUNCTIONS_COUNT][2];
extern PCHAR beacon_compatibility_output;
extern INT   beacon_compatibility_size;
extern INT   beacon_compatibility_offset;
extern PBYTE beacon_compatibility_filecontent;
extern SIZE_T beacon_compatibility_filesize;
extern SIZE_T beacon_compatibility_allocsize;

typedef struct {
    PCHAR original;  /* the original buffer [so we can free it] */
    PCHAR buffer;    /* current pointer into our buffer */
    INT    length;   /* remaining length of data */
    INT    size;     /* total size of this buffer */
} datap;

typedef struct {
    PCHAR original;  /* the original buffer [so we can free it] */
    PCHAR buffer;    /* current pointer into our buffer */
    INT    length;   /* remaining length of data */
    INT    size;     /* total size of this buffer */
} formatp;

/* Data Parsing Functions */
VOID    beacon_data_parse(datap * parser, char * buffer, int size);
INT     beacon_data_int(datap * parser);
SHORT   beacon_data_short(datap * parser);
INT     beacon_data_length(datap * parser);
PCHAR   beacon_data_extract(datap * parser, int * size);
/* Format Functions */
VOID    beacon_format_alloc(formatp * format, int maxsz);
VOID    beacon_format_reset(formatp * format);
VOID    beacon_format_free(formatp * format);
VOID    beacon_format_append(formatp * format, char * text, int len);
VOID    beacon_format_printf(formatp * format, char * fmt, ...);
PCHAR   beacon_format_to_string(formatp * format, int * size);
VOID    beacon_format_int(formatp * format, int value);

/* Output Functions */
VOID    beacon_printf(INT type, PCHAR fmt, ...);
VOID    beacon_output(INT type, PCHAR data, SIZE_T len);

/* Token Functions */
BOOL   beacon_use_token(HANDLE token);
VOID   beacon_revert_token();
BOOL   beacon_is_admin();

/* Spawn+Inject Functions */
VOID   beacon_get_spawn_to(BOOL x86, PCHAR buffer, INT length);
BOOL   beacon_spawn_temporary_process(BOOL x86, BOOL ignoreToken, STARTUPINFO * sInfo, PROCESS_INFORMATION * pInfo);
VOID   beacon_inject_process(HANDLE hProc, INT pid, PCHAR payload, INT p_len, INT p_offset, PCHAR arg, INT a_len);
VOID   beacon_inject_temporary_process(PROCESS_INFORMATION * pInfo, PCHAR payload, INT p_len, INT p_offset, PCHAR arg, INT a_len);
VOID   beacon_cleanup_process(PROCESS_INFORMATION * pInfo);
/* Utility Functions */
BOOL   to_wide_char(PCHAR src, wchar_t * dst, INT max);
UINT32 swap_endianess(UINT32 indata);

PBYTE beacon_get_output_data(INT *outsize);
#endif
