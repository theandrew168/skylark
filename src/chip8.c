#include "chip8.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "graphics.h"
#include "input.h"

#define MEM_SIZE 4096
#define REG_SIZE 16
#define STK_SIZE 16
#define KEY_SIZE 16
#define PC_START 0x200

#define WINDOW_TITLE "Skylark Chip8 Emulator"
#define PIXEL_SIZE 16

#define DEBUG 0

/*
 * 2-byte opcodes
 */
typedef unsigned short opcode_t;

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
 */
typedef struct chip8_t {
    unsigned char memory[MEM_SIZE];
    unsigned char V[REG_SIZE];
    unsigned short I;
    unsigned short pc;

    unsigned short stack[STK_SIZE];
    unsigned short sp;

    unsigned char keys[KEY_SIZE];

    unsigned char pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    unsigned char delay_timer;
    unsigned char sound_timer;
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
static unsigned char chip8_fontset[80] = { 
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

static void dump_registers() {
    for (int i = 0; i < REG_SIZE; ++i) {
        if (chip8.V[i] == 0) continue;

        printf("V[%d]:\t %d\n", i, chip8.V[i]);
    }
    printf("I:\t %d\n", chip8.I);
    printf("pc:\t %d\n", chip8.pc);
}

static void reset_state() {
    memset(chip8.memory, 0, MEM_SIZE * sizeof(unsigned char));
    memset(chip8.V, 0, REG_SIZE * sizeof(unsigned char));
    
    chip8.I = 0;
    chip8.pc = PC_START;

    memset(chip8.stack, 0, STK_SIZE * sizeof(unsigned short));
    chip8.sp = 0;

    memset(chip8.keys, 0, KEY_SIZE * sizeof(unsigned char));

    memset(chip8.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(unsigned char));
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;

    /* Place fontset into chip8 memory */
    for (int i = 0; i < 80; ++i) {
        chip8.memory[i] = chip8_fontset[i];
    }

    /* Seed the RNG */
    srand(time(NULL));
}

static int get_pressed_key() {
    for (int i = 0; i < KEY_SIZE; ++i) {
        if (chip8.keys[i]) return i;
    }

    return -1;
}

bool chip8_init() {
    reset_state();

    if (!graphics_init(WINDOW_TITLE, PIXEL_SIZE)) {
        printf("Failed to init graphics\n");
        return false;
    }

    input_init();
    return true;
}

void chip8_terminate() {
    graphics_terminate();
}

bool chip8_running() {
    return !input_close_requested();
}

bool chip8_load_rom(const char* rom) {
    int i, length, bytes_read;
    unsigned char* buffer;

    FILE* file = fopen(rom, "rb");
    if (!file) {
        printf("Unable to locate ROM: %s\n", rom);
        return false;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);

    buffer = (unsigned char*)malloc(sizeof(unsigned char) * length);
    if (!buffer) {
        printf("Failed to allocate memory for ROM: %s\n", rom);
        fclose(file);
        return false;
    }

    bytes_read = fread(buffer, 1, length, file);
    if (bytes_read != length) {
        printf("Failed to read ROM: %s\n", rom);
        free(buffer);
        fclose(file);
        return false;
    }

    for (i = 0; i < length; ++i) {
        chip8.memory[PC_START + i] = buffer[i];
    }

    fclose(file);
    free(buffer);

    return true;
}

static void pre_cycle() {
    input_update();
    for (int i = 0; i < 16; ++i) {
        chip8.keys[i] = input_is_key_down(i);
    }

    graphics_clear_screen();
}

static void post_cycle() {
    graphics_draw();

    if (chip8.delay_timer > 0) {
        --chip8.delay_timer;
    }
        
    if (chip8.sound_timer > 0) {
        printf("BEEP\n");
        --chip8.sound_timer;
    }
}

void chip8_emulate_cycle() {
    opcode_t opcode;
    unsigned short nnn, kk, x, y, n;
    int i, dx, dy;

    pre_cycle();

    if (DEBUG) dump_registers();

    /* Parse out relevant fields */
    opcode = chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1];
    nnn = opcode & 0x0FFF;
    kk = opcode & 0x00FF;
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    n = opcode & 0x000F;

//    printf("Opcode: %x\n", opcode);
//    printf("%d, %d, %d\n", chip8.I, chip8.pc, chip8.sp);
//    printf("%d, %d, %d, %d, %d, %d, %d, %d, ",
//           chip8.V[0],chip8.V[1],chip8.V[2],chip8.V[3],
//           chip8.V[4],chip8.V[5],chip8.V[6],chip8.V[7]);
//    printf("%d, %d, %d, %d, %d, %d, %d, %d\n",
//           chip8.V[8],chip8.V[9],chip8.V[10],chip8.V[11],
//           chip8.V[12],chip8.V[13],chip8.V[14],chip8.V[15]);


    /* Decode instruction */
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: /* 00E0 - CLS */
                    graphics_clear_pixels();
                    break;
                case 0x00EE: /* 00EE - RET */
                    --chip8.sp;
                    chip8.pc = chip8.stack[chip8.sp];
                    break;
                default:     /* 0nnn - JP addr */
                    chip8.pc = nnn;
                    chip8.pc -= 2;
            }
            break;
        case 0x1000: /* 1nnn - JP addr */
            chip8.pc = nnn;
            chip8.pc -= 2;
            break;
        case 0x2000: /* 2nnn - CALL addr */
            chip8.stack[chip8.sp] = chip8.pc;
            ++chip8.sp;
            chip8.pc = nnn;
            chip8.pc -= 2;
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
            chip8.V[x] += kk;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: /* 8xy0 - LD Vx, Vy */
                    chip8.V[x] = chip8.V[y];
                    break;
                case 0x0001: /* 8xy1 - OR Vx, Vy */
                    chip8.V[x] |= chip8.V[y];
                    break;
                case 0x0002: /* 8xy2 - AND Vx, Vy */
                    chip8.V[x] &= chip8.V[y];
                    break;
                case 0x0003: /* 8xy3 - XOR Vx, Vy */
                    chip8.V[x] ^= chip8.V[y];
                    break;
                case 0x0004: /* 8xy4 - ADD Vx, Vy */
                    chip8.V[0xF] = (chip8.V[x] > 0xFF - chip8.V[y]) ? 1 : 0;
                    chip8.V[x] += chip8.V[y];
                    break;
                case 0x0005: /* 8xy5 - SUB Vx, Vy */
                    chip8.V[0xF] = (chip8.V[x] < chip8.V[y]) ? 1 : 0;
                    chip8.V[x] -= chip8.V[y];
                    break;
                case 0x0006: /* 8xy6 - SHR Vx {, Vy} */
                    chip8.V[0xF] = chip8.V[x] & 0x01;
                    chip8.V[x] >>= 1;
                    break;
                case 0x0007: /* 8xy7 - SUBN Vx, Vy */
                    chip8.V[0xF] = (chip8.V[y] < chip8.V[x]) ? 1 : 0;
                    chip8.V[x] = chip8.V[y] - chip8.V[x];
                    break;
                case 0x000E: /* 8xyE - SHL Vx {, Vy} */
                    chip8.V[0xF] = chip8.V[x] >> 7;
                    chip8.V[x] <<= 1;
                    break;
            }
            break;
        case 0x9000: /* 9xy0 - SNE Vx, Vy */
            if (chip8.V[x] != chip8.V[y]) chip8.pc += 2;
            break;
        case 0xA000: /* Annn - LD I, addr */
            chip8.I = nnn;
            break;
        case 0xB000: /* Bnnn - JP V0, addr */
            chip8.pc = nnn + chip8.V[0];
            chip8.pc -= 2;
            break;
        case 0xC000: /* Cxkk - RND Vx, byte */
            chip8.V[x] = ((unsigned char)(rand() % 0xFF)) & kk;
            break;
        case 0xD000: /* Dxyn - DRW Vx, Vy, nibble */
            chip8.V[0xF] = 0;
            for (dy = 0; dy < n; ++dy) {
                for (dx = 0; dx < 8; ++dx) {
                    unsigned short pixel = chip8.memory[chip8.I + dy];
                    if (!(pixel & (0x80 >> dx))) continue;

                    /* Update pixel and check collision */
                    if (graphics_set_pixel(
                            //(7 - dx) + chip8.V[x],
                            dx + chip8.V[x],
                            dy + chip8.V[y], 1)) {
                        chip8.V[0xF] = 1;
                    }
                }
            }
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: /* Ex9E - SKP Vx */
                    if (chip8.keys[chip8.V[x]]) chip8.pc += 2;
                    break;
                case 0x00A1: /* ExA1 - SKNP Vx */
                    if (!chip8.keys[chip8.V[x]]) chip8.pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: /* Fx07 - LD Vx, DT */
                    chip8.V[x] = chip8.delay_timer;
                    break;
                case 0x000A: /* Fx0A - LD Vx, K */
                    printf("wait for key\n");
                    if (get_pressed_key() < 0) return;
                    chip8.V[x] = (unsigned char)get_pressed_key();
                    break; 
                case 0x0015: /* Fx15 - LD DT, Vx */
                    chip8.delay_timer = chip8.V[x];
                    break;
                case 0x0018: /* Fx18 - LD ST, Vx */
                    chip8.sound_timer = chip8.V[x];
                    break;
                case 0x001E: /* Fx1E - ADD I, Vx */
                    chip8.I += chip8.V[x];
                    break;
                case 0x0029: /* Fx29 - LD F, Vx */
                    chip8.I = x * 5;
                    break;
                case 0x0033: /* Fx33 - LD B, Vx */
                    chip8.memory[chip8.I]     = (chip8.V[x] / 100);
                    chip8.memory[chip8.I + 1] = (chip8.V[x] / 10)  % 10;
                    chip8.memory[chip8.I + 2] = (chip8.V[x] % 100) % 10;
                    break;
                case 0x0055: /* Fx55 - LD [I], Vx */
                    for (i = 0; i <= x; ++i) {
                        chip8.memory[chip8.I + i] = chip8.V[i];
                    }
                    chip8.I += x + 1;
                    break;
                case 0x0065: /* Fx65 - LD Vx, [I] */
                    for (i = 0; i <= x; ++i) {
                        chip8.V[i] = chip8.memory[chip8.I + i];
                    }
                    chip8.I += x + 1;
                    break;
            }
            break;
    }

    chip8.pc += 2;

    post_cycle();
}
