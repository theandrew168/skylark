#include "input.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include "types.h"

#define NUM_KEYS 16

static SDL_Keycode gamepad[NUM_KEYS] = {
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v
};

typedef struct input_t {
    bool keys[NUM_KEYS];
    bool closeRequested;    
} input_t;

static input_t input;
static SDL_Event event;

void input_init() {
    memset(input.keys, 0, NUM_KEYS);
    input.closeRequested = false;
}   

void input_update() {
    size_t i;
    memset(input.keys, 0, NUM_KEYS);

    /* Poll events from window */
    while (SDL_PollEvent(&event)) {
        /* Check for closing the window */
        if (event.type == SDL_QUIT) {
            input.closeRequested = true;
            return;
        }

        /* Check for each key in the 16-button keypad */
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_ESCAPE) {
                input.closeRequested = true;
                return;
            }

            for (i = 0; i < NUM_KEYS; i++) {
                if (key == gamepad[i]) {
                    input.keys[i] = true;
                }
            }
        }
    } 
}

bool input_closeRequested() {
    return input.closeRequested;
}

bool input_isKeyDown(uint8 key) {
    if (key >= NUM_KEYS) {
        printf("Invalid key: %u", key);
        return false;
    }

    return input.keys[key];
}
