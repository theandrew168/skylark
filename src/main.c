#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "chip8.h"

enum {
    SKYLARK_DISPLAY_PIXEL_SIZE = 16,
};

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

    struct chip8 chip8 = { 0 };
    chip8_init(&chip8);

    int rc = chip8_load(&chip8, buf, size);
    if (rc != CHIP8_OK) {
        free(buf);
        fprintf(stderr, "failed to init chip8 emulator\n");
        return EXIT_FAILURE;
    }
    free(buf);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to init SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Skylark CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_DISPLAY_WIDTH * SKYLARK_DISPLAY_PIXEL_SIZE,
        CHIP8_DISPLAY_HEIGHT * SKYLARK_DISPLAY_PIXEL_SIZE,
        SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "failed to create SDL2 window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "failed to create SDL2 renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }


    bool running = true;
    while (running) {
        // input
        SDL_Event event = { 0 };
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) running = false;
            }
        }

        // execute the next instruction
        rc = chip8_step(&chip8);
        if (rc != CHIP8_OK) {
            running = false;
            break;
        }

        // graphics
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (long y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
            for (long x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
                if (!chip8_pixel(&chip8, x, y)) continue;

                SDL_Rect pixel = {
                    .w = SKYLARK_DISPLAY_PIXEL_SIZE,
                    .h = SKYLARK_DISPLAY_PIXEL_SIZE,
                    .x = x * SKYLARK_DISPLAY_PIXEL_SIZE,
                    .y = y * SKYLARK_DISPLAY_PIXEL_SIZE,
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
        SDL_RenderPresent(renderer);

        // TODO sound?
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
