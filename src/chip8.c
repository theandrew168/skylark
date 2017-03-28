#include "chip8.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * 2-byte opcodes
 */
typedef uint16_t opcode_t;

/*
 * 4K memory
 * 16 8-bit registers
 * Index register
 * Program counter
 *
 * Program stack (size 16)
 * Stack pointer
 *
 * 16-key input status
 *
 * 2048 (64 * 32) pixels
 * Delay timer
 * Sound timer
 *
 * Draw requested flag
 * Screen clear requested flag
 */
typedef struct chip8_t {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;

    uint16_t stack[16];
    uint16_t sp;

    uint8_t keys[16];

    uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t timer_delay;
    uint8_t timer_sound;

    bool drawRequested;
    bool clearRequested;
} chip8_t;


/*
 * Chip8 font information
 * 'A', for example:
 * 11110000
 * 10010000
 * 11110000
 * 10010000
 * 10010000
 */
static uint8_t chip8_fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

static chip8_t chip8;

void chip8_init() {
    size_t i;

    memset(chip8.memory, 0, 4096);
    memset(chip8.V, 0, 16);
    
    chip8.I = 0;
    chip8.pc = 0x200;

    memset(chip8.stack, 0, 16);
    chip8.sp = 0;

    memset(chip8.keys, 0, 16);

    memset(chip8.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
    chip8.timer_delay = 0;
    chip8.timer_sound = 0;

    chip8.drawRequested = false;
    chip8.clearRequested = false;

    /* Place fontset into chip8 memory */
    for (i = 0; i < 80; i++) {
        chip8.memory[i] = chip8_fontset[i];
    }
}

void chip8_terminate() {
    /* Nothing to do here? */
}

void chip8_loadROM(const char* rom) {
    size_t i, length, bytes_read;
    uint8_t* buffer;

    FILE* file = fopen(rom, "rb");
    if (!file) {
        printf("Unable to locate ROM: %s\n", rom);
        return;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);

    buffer = (uint8_t*)malloc(sizeof(uint8_t) * length);
    if (!buffer) {
        printf("Failed to allocate memory for ROM: %s\n", rom);
        fclose(file);
        return;
    }

    bytes_read = fread(buffer, 1, length, file);
    if (bytes_read != length) {
        printf("Failed to read ROM: %s\n", rom);
        free(buffer);
        fclose(file);
    }

    if (length < (0xFFF - 0x200)) {
        for (i = 0; i < length; i++) {
            chip8.memory[0x200 + i] = buffer[i];
        }
    } else {
        printf("ROM too big for memory: %s\n", rom);
    }

    fclose(file);
    free(buffer);
}

void chip8_emulateCycle() {
    opcode_t opcode;
    uint16_t nnn, kk, x, y; /* n; */

    /* Parse out relevant fields */
    opcode = chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1];
    nnn = opcode & 0x0FFF;
    kk = opcode & 0x00FF;
    x = opcode & 0x0F00;
    y = opcode & 0x00F0;
    /* n = opcode & 0x000F; */

    /* Decode instruction */
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: /* 00E0 - CLS */
                    chip8.clearRequested = true;
                    break;
                case 0x00EE: /* 00EE - RET */
                    chip8.pc = chip8.stack[chip8.sp--];
                    break;
                default:     /* 0nnn - JP addr */
                    chip8.pc = nnn;
            }
            break;
        case 0x1000: /* 1nnn - JP addr */
            chip8.pc = nnn;
            break;
        case 0x2000: /* 2nnn - CALL addr */
            chip8.stack[++chip8.sp] = chip8.pc;
            chip8.pc = nnn;
            break;
        case 0x3000: /* 3xkk - SE Vx, byte */
            if (chip8.V[x] == kk) chip8.pc += 2;
            break;
        case 0x4000: /* 4xkk - SNE Vx, byte */
            if (chip8.V[x] != kk) chip8.pc += 2;
            break;
        case 0x5000: /* 5xy0 - SE Vx, Vy */
            if (chip8.V[x] != chip8.V[y]) chip8.pc += 2;
            break;
        case 0x6000: /* 6xkk - LD Vx, byte */
            chip8.V[x] = kk;
            break;
        case 0x7000: /* 7xkk - ADD Vx, byte */
            chip8.V[x] = chip8.V[x] + kk;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: /* 8xy0 - LD Vx, Vy */
                    chip8.V[x] = chip8.V[y];
                    break;
                case 0x0001: /* 8xy1 - OR Vx, Vy */
                    break;
                case 0x0002: /* 8xy2 - AND Vx, Vy */
                    break;
                case 0x0003: /* 8xy3 - XOR Vx, Vy */
                    break;
                case 0x0004: /* 8xy4 - ADD Vx, Vy */
                    break;
                case 0x0005: /* 8xy5 - SUB Vx, Vy */
                    break;
                case 0x0006: /* 8xy6 - SHR Vx {, Vy} */
                    break;
                case 0x0007: /* 8xy7 - SUBN Vx, Vy */
                    break;
                case 0x000E: /* 8xyE - SHL Vx {, Vy} */
                    break;
            }
            break;
        case 0x9000: /* 9xy0 - SNE Vx, Vy */
            break;
        case 0xA000: /* Annn - LD I, addr */
            break;
        case 0xB000: /* Bnnn - JP V0, addr */
            break;
        case 0xC000: /* Cxkk - RND Vx, byte */
            break;
        case 0xD000: /* Dxyn - DRW Vx, Vy, nibble */
            break;
        case 0xE000: /* Exxx group */
            break;
        case 0xF000: /* Fxxx group */
            break;
    }
}

void chip8_updateInput() {

}

bool chip8_drawRequested() {
    if (chip8.drawRequested) {
        chip8.drawRequested = false;
        return true;
    } else {
        return false;
    }
}

bool chip8_clearRequested() {
    if (chip8.clearRequested) {
        chip8.clearRequested = false;
        return true;
    } else {
        return false;
    }
}
