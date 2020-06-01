#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "isa.h"

// TODO: This impl is too naive. It needs to simulate execution of the
// ROM in order to know how instructions are aligned. Unfortunately,
// the instructions in these ROMs don't always align on the 2-byte
// boundary (due to game data being intermixed). Additionally,
// the misc bits of game data could easily overlap with a valid
// instruction. This means that you can't just check if every 2-byte
// segment is a valid instruction or not.
//
// I'll need to make some sort of DAG pass through the ROM and mark
// which sections of the memory get executed as instructions. Then,
// I can iterate back through and print the instructions and data.
// Both sides of every branch will need to be explored.
//
// TODO: Make a blog post about this.

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <rom_file>\n", argv[0]);
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

    struct instruction inst = { 0 };
    for (long i = 0; i < size; i += 2) {
        printf("0x%04lx | ", i);
        uint16_t code = buf[i] << 8 | buf[i + 1];
        isa_instruction_decode(&inst, code);
        printf("TODO\n");
    }

    free(buf);
    return EXIT_SUCCESS;
}
