#ifndef GAME_H
#define GAME_H

#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// #include <glib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "cgl.h"
#include "utils.h"

// Struct containing game Variables
struct Game {
  // Cells Arrays
  int CELLS_SIZE;

  // Grid init
  int grid_cell_size;
  int offset_x;
  int offset_y;

  // SDL Variables
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_DisplayMode dm;

  // Game Variables
  bool simulate;
  bool running;
  bool hashlife;

  // legacy
  int grid_height;
};

// Init
struct Game *alloc_Game(void);

struct Game *g_start(int cells, bool useHashlife);

void g_close(struct Game *g);

void g_fillRandom();

// execute this function while Game.running is true
void g_loop(struct Game *g);

// Output
void g_draw(struct Game *g);

void g_drawToText(struct Game *g, char *filename);

// Simulate a set number of generations and output to text  file
void g_sim_gens_to_txt(int cells_size, int gens, char *file, bool fillRandom,
                       bool useHashlife);

cgl_data_t *g_getData();

// define colors
extern SDL_Color alive_color;
extern SDL_Color grid_line_color;

#endif