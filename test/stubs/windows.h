#ifndef TEST_WINDOWS_STUB_H
#define TEST_WINDOWS_STUB_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef int32_t INT32;
typedef int16_t INT16;
typedef unsigned long long UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef unsigned int UINT;
typedef int INT;
typedef short SHORT;
typedef void VOID;
typedef size_t SIZE_T;
typedef void* PVOID;
typedef const void* PCVOID;
typedef char CHAR;
typedef CHAR* PCHAR;
typedef const CHAR* PCCHAR;
typedef const CHAR* LPCSTR;
typedef CHAR* LPSTR;
typedef wchar_t WCHAR;
typedef WCHAR* PWCHAR;
typedef const WCHAR* LPCWSTR;
typedef BYTE* PBYTE;
typedef DWORD* PDWORD;
typedef INT32* PINT32;
typedef INT32* PINT;
typedef SHORT* PSHORT;
typedef BOOL* PBOOL;
typedef BYTE* LPBYTE;
typedef void* HANDLE;
typedef HANDLE HMODULE;
typedef void* FARPROC;
typedef HANDLE* PHANDLE;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define TRUE 1
#define FALSE 0

#define WINAPI

#define HEAP_ZERO_MEMORY 0x00000008
#define CREATE_NO_WINDOW 0x08000000

#define CP_ACP 0
#define MB_ERR_INVALID_CHARS 0x00000008

#define SECURITY_BUILTIN_DOMAIN_RID 0x00000020
#define DOMAIN_ALIAS_RID_ADMINS     0x00000220
#define SECURITY_NT_AUTHORITY      {0,0,0,0,0,5}

#define IMAGE_SYM_CLASS_EXTERNAL      0x02
#define IMAGE_SYM_CLASS_EXTERNAL_DEF  0x05

#define LOCALE_INVARIANT 0
#define CSTR_EQUAL 2

typedef struct _SID_IDENTIFIER_AUTHORITY {
    BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

typedef PVOID PSID;

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD  wShowWindow;
    WORD  cbReserved2;
    BYTE* lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFO, *LPSTARTUPINFO;

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD  dwProcessId;
    DWORD  dwThreadId;
} PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef FARPROC (WINAPI *PROC_ADDRESS_FN)(HMODULE, LPCSTR);

#define RtlMoveMemory(Destination, Source, Length) memmove((Destination), (Source), (Length))
#define RtlZeroMemory(Destination, Length) memset((Destination), 0, (Length))

int      lstrlenA(LPCSTR string);
int      lstrcmpA(LPCSTR string1, LPCSTR string2);
int      CompareStringA(int locale, DWORD flags, LPCSTR string1, int count1, LPCSTR string2, int count2);
int      wvsprintfA(LPSTR buffer, LPCSTR format, va_list args);
int      MultiByteToWideChar(UINT codePage, DWORD flags, LPCSTR src, int srcLength, PWCHAR dest, int destLength);

HANDLE   GetProcessHeap(void);
PVOID    HeapAlloc(HANDLE heap, DWORD flags, SIZE_T size);
PVOID    HeapReAlloc(HANDLE heap, DWORD flags, PVOID memory, SIZE_T size);
BOOL     HeapFree(HANDLE heap, DWORD flags, PVOID memory);

HMODULE  GetModuleHandleA(LPCSTR moduleName);
HMODULE  LoadLibraryA(LPCSTR moduleName);
FARPROC  GetProcAddress(HMODULE module, LPCSTR procedureName);
BOOL     FreeLibrary(HMODULE module);

BOOL     CreateProcessA(LPCSTR applicationName,
                        LPSTR commandLine,
                        PVOID processAttributes,
                        PVOID threadAttributes,
                        BOOL inheritHandles,
                        DWORD creationFlags,
                        PVOID environment,
                        LPCSTR currentDirectory,
                        STARTUPINFO* startupInfo,
                        PROCESS_INFORMATION* processInformation);
BOOL     CloseHandle(HANDLE object);

BOOL     SetThreadToken(PHANDLE thread, HANDLE token);
BOOL     RevertToSelf(void);
BOOL     AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY authority,
                                  BYTE subAuthorityCount,
                                  DWORD subAuthority0,
                                  DWORD subAuthority1,
                                  DWORD subAuthority2,
                                  DWORD subAuthority3,
                                  DWORD subAuthority4,
                                  DWORD subAuthority5,
                                  DWORD subAuthority6,
                                  DWORD subAuthority7,
                                  PSID* sid);
BOOL     CheckTokenMembership(HANDLE tokenHandle, PSID sidToCheck, PBOOL isMember);
void     FreeSid(PSID sid);

void     __C_specific_handler(void);

#endif /* TEST_WINDOWS_STUB_H */
