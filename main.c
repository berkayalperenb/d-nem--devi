/*
 * Ad Soyad   : Berkay Alperen Bayram
 * Ogrenci No : 439541
 */
#include "parser.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: numtool <input_file> <output_file>\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "ERROR: cannot open input file: %s\n", argv[1]);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "ERROR: cannot open output file: %s\n", argv[2]);
        fclose(in);
        return 1;
    }

    RecordList records;
    records_init(&records);

    if (!process_input(in, out, &records)) {
        fprintf(stderr, "ERROR: processing failed (memory allocation)\n");
        records_free(&records);
        fclose(in);
        fclose(out);
        return 1;
    }

    records_free(&records);
    fclose(in);
    fclose(out);
    return 0;
}
