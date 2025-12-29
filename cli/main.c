#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "run_coff.h"

int main(int argc, char *argv[]) {
    PCHAR coff_data   = NULL;
    DWORD coff_size   = 0;

    PCHAR param_data  = NULL;
    DWORD param_size  = 0;

    PCHAR out_data    = NULL;
    INT out_data_size = 0;

    PBYTE out_file       = NULL;
    SIZE_T out_file_size = 0;

    if (argc < 2) {
        printf("Usage: %s <file_path> param\n", argv[0]);
        return 1;
    }

    coff_data = (PCHAR)ReadFileToBuffer(argv[1], &coff_size);
    if (!coff_data) {
        return 1;
    }

    if (argc >= 3) {
        param_data = unhexlify(argv[2], &param_size);
        if (!param_data) {
            return 1;
        }
    }


    for(int i = 0; i < 1; i++) {
        int toto = run_coff(coff_data, coff_size, param_data, param_size);

        if (toto == 0) {
            _inf_coffloader("Ran/parsed the coff");
            out_data = beacon_get_output_data(&out_data_size);
            if (out_data != NULL) {

                    printf("Outdata Below:\n%s %d", out_data, out_data_size);
                    mcfree(out_data);
            }

            beacon_get_file_data(&out_file, &out_file_size);
            if (out_file != NULL) {
                    printf("Outfile Below: size %zu\n", out_file_size);
                    
                    // For demonstration, we write the output file to disk
                    FILE *fp = fopen("output_file.bin", "wb");
                    fwrite(out_file, 1, out_file_size, fp);
                    fclose(fp);

                    beacon_free_file_data();
            }

        }
    }

    mcfree(coff_data);
    mcfree(param_data);

    return 0;
}
