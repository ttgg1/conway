#ifndef GAME_H
#define GAME_H

#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <SDL2/SDL_video.h>

// Divide and round to closest
#define DIV_ROUND_CLOSEST(n, d)                                                \
  ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d) / 2) / (d)) : (((n) + (d) / 2) / (d)))

// Struct containing game Variables
typedef struct Game{
  // Cells Arrays
  unsigned int CELLS_SIZE;
  unsigned int NUM_THREADS;
  bool *cells;
  bool *cells_next;
  unsigned int arr_size;

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

  //legacy
  int grid_width;
  int grid_height;
  int window_width;
  int window_height;
} Game;

// Game Functions
extern void draw(Game *g);
extern void *getNeighbours(void* p); // executed using threads
extern void tick(Game *g);
extern void loop(Game *g);
extern int init(Game *g,const int num_cells,const int num_threads);
extern void close(Game *g);

// Helper Functions
extern bool isAlive(Game *g, int index);
extern bool isAlive_thread(bool* cells, int index, int arr_size); //overload for internal use
extern void flipCell(Game *g, int index);
extern int getIndex(Game *g,int x,int y);
extern bool getCellAt(Game *g,int x,int y);

extern int modulo(int a, int b);

// define colors
extern SDL_Color back_color;
extern SDL_Color line_color;
extern SDL_Color alive_color;
extern SDL_Color grid_line_color;

#endif