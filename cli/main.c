#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "run_coff.h"

int main(int argc, char *argv[]) {
    PCHAR coff_data  = NULL;
    DWORD coff_size  = 0;
    PCHAR param_data = NULL;
    DWORD param_size = 0;

    if (argc < 3) {
        printf("Usage: %s <file_path> param\n", argv[0]);
        return 1;
    }

    coff_data = (PCHAR)ReadFileToBuffer(argv[1], &coff_size);
    if (!coff_data) {
        return 1;
    }

    param_data = unhexlify(argv[2], &param_size);
    if (!param_data) {
        return 1;
    }


    int toto = run_coff(coff_data, coff_size, param_data, param_size);


    return 0;
}