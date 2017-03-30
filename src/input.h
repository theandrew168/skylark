#ifndef SKYLARK_INPUT_H
#define SKYLARK_INPUT_H

#include <SDL2/SDL.h>
#include "types.h"

void input_init();
void input_update();
bool input_close_requested();
bool input_is_key_down(int key);

#endif
