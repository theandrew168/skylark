#ifndef SKYLARK_CHIP8_H
#define SKYLARK_CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "graphics.h"

void chip8_init();
void chip8_terminate();
void chip8_loadROM(const char* rom);
void chip8_emulateCycle();
void chip8_updateInput();
bool chip8_drawRequested();

#endif
