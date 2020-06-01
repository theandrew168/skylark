#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const long BYTES_PER_ROW = 8;

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <rom_file> <array_name>\n", argv[0]);
        fprintf(stderr, "  (ex: %s roms/maze.rom ROM_MAZE)\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "failed to open rom: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t* buf = malloc(size);
    if (buf == NULL) {
        fclose(fp);
        fprintf(stderr, "failed to allocate buffer to hold ROM contents\n");
        return EXIT_FAILURE;
    }

    long count = fread(buf, 1, size, fp);
    if (count != size) {
        free(buf);
        fclose(fp);
        fprintf(stderr, "failed to read ROM into buffer\n");
        return EXIT_FAILURE;
    }
    fclose(fp);

    printf("#include <stdint.h>\n");
    printf("const uint8_t %s[] = {\n", argv[2]);
    for (long i = 0; i < size; i++) {
        if (i % BYTES_PER_ROW == 0) printf("    ");
        printf("0x%02x, ", buf[i]);
        if (i % BYTES_PER_ROW == BYTES_PER_ROW - 1) printf("\n");
    }
    printf("\n};\n");

    free(buf);
    return EXIT_SUCCESS;
}
