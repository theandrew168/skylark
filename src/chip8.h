#ifndef SKYLARK_CHIP8_H
#define SKYLARK_CHIP8_H

#include "types.h"
#include "graphics.h"

void chip8_init();
void chip8_terminate();
bool chip8_load_rom(const char* rom);
void chip8_emulate_cycle();
void chip8_update_input();
bool chip8_draw_requested();
bool chip8_clear_requested();

#endif
