#ifndef SKYLARK_CHIP8_H
#define SKYLARK_CHIP8_H

#include "types.h"
#include "graphics.h"

bool chip8_init();
void chip8_terminate();
bool chip8_running();
bool chip8_load_rom(const char* rom);
void chip8_emulate_cycle();

#endif
