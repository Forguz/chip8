#include "SDL2/SDL_events.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_timer.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// SDL Container object
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} sdl_t;

// Emulator configuration object
typedef struct {
  uint32_t window_width;  // SDL window width
  uint32_t window_height; // SDL window height
  uint32_t fg_color;      // Foreground color RGBA8888
  uint32_t bg_color;      // Background color RGBA8888
  uint32_t scale_factor;  // Amount to scale a CHIP8 pixel by e.g. 20x will be a
                          // 20x larger window
} config_t;

// Emulator states
typedef enum {
  QUIT,
  RUNNING,
  PAUSED,
} emulator_state_t;

typedef struct {
  emulator_state_t state;
} chip8_t;

bool init_sdl(sdl_t *sdl, const config_t config) {
  bool validation = (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
  if (SDL_InitSubSystem(validation) != false) {
    SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
    return false;
  }

  sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 config.window_width * config.scale_factor,
                                 config.window_height * config.scale_factor, 0);

  if (!sdl->window) {
    SDL_Log("Could not create SDL window %s", SDL_GetError());
    return false;
  }

  sdl->renderer = SDL_CreateRenderer(sdl->window, 3, SDL_RENDERER_ACCELERATED);

  if (!sdl->renderer) {
    SDL_Log("Could not create SDL renderer %s", SDL_GetError());
    return false;
  }

  return true;
}

bool set_config_from_args(config_t *config, const int argc, char **argv) {
  *config = (config_t){
      .window_width = 64,     // CHIP8 original X resolution
      .window_height = 32,    // CHIP8 original Y resolution
      .fg_color = 0xFFFFFFFF, // YELLOW
      .bg_color = 0xFFFF00FF, // BLACK
      .scale_factor = 20,     // Default resolution will be 1280x640
  };

  for (int i = 1; i < argc; i++) {
    (void)argv[i]; // Prevent unused variables error;
  }
  return true;
}

bool init_chip8(chip8_t *chip8) {
  chip8->state = RUNNING;
  return true;
}

void final_cleanup(const sdl_t sdl) {
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}

void clear_screen(const sdl_t sdl, const config_t config) {
  const uint8_t r = (config.bg_color >> 24) & 0xFF; // 0xFFFF00FF -> 0x000000FF
  const uint8_t g = (config.bg_color >> 16) & 0xFF; // 0x0000FFFF
  const uint8_t b = (config.bg_color >> 8) & 0xFF;  // 0x00FFFF00
  const uint8_t a = config.bg_color & 0xFF;         // 0x0FFFF00F

  SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
  SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl) { SDL_RenderPresent(sdl.renderer); }

void handle_input(chip8_t *chip8) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      printf("%i \n", SDL_QUIT);
      // Exit window; End program
      chip8->state = QUIT;
      return;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        chip8->state = QUIT;
        return;
      }
      break;
    case SDL_KEYUP:
      break;
    }
  }
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // Initialize emulator configuration/options
  config_t config = {0};
  if (!set_config_from_args(&config, argc, argv))
    exit(EXIT_FAILURE);
  // Initialize SDL
  sdl_t sdl = {0};
  if (!init_sdl(&sdl, config))
    exit(EXIT_FAILURE);

  // Initialize CHIP8 machine
  chip8_t chip8 = {0};
  if (!init_chip8(&chip8))
    exit(EXIT_FAILURE);

  // Initial screen clear
  clear_screen(sdl, config);

  while (chip8.state != QUIT) {
    handle_input(&chip8);
    // Get_time();
    // Emulate CHIP8 instructions
    // Get_time() elapsed since last get_time();
    // Update window with changes
    update_screen(sdl);
    // // Delay for appr 60hz/60fps (16.67ms)
    SDL_Delay(16); // - actual time elapsed
  }

  final_cleanup(sdl);

  printf("THIS LOADS");
  exit(EXIT_SUCCESS);
}
