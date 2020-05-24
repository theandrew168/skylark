#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("usage: %s <rom_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("failed to open rom: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void* buf = malloc(size);
    if (buf == NULL) {
        printf("failed to allocate buffer to hold ROM contents\n");
        return EXIT_FAILURE;
    }

//    if (fread(dest,

    fclose(fp);
    return EXIT_SUCCESS;
}
