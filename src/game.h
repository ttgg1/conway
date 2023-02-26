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

// Game Functions
extern void draw();
extern void *getNeighbours(void* p);
extern void tick();
extern void loop();
extern int init(const int num_cells,const int num_threads);
extern void close();

// Helper Functions
extern bool isAlive(bool *cells, int index);
extern void flipCell(bool *cells, int index);
extern int getIndex(int x,int y);
extern bool getCellAt(bool* cells,int x,int y);

extern int modulo(int a, int b);

// Constants
extern unsigned int CELLS_SIZE;
extern unsigned int NUM_THREADS;

// Variables

// Cells Arrays
extern bool *g_cells;
extern bool *g_cells_next;
extern unsigned int arr_size;

// Grid init
extern int grid_cell_size;
extern int offset_x;
extern int offset_y;

// legacy
extern int grid_width;
extern int grid_height;
extern int window_width;
extern int window_height;

// define colors
extern SDL_Color back_color;
extern SDL_Color line_color;
extern SDL_Color alive_color;
extern SDL_Color grid_line_color;

// SDL Variables
extern bool running;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_DisplayMode dm;

// Game Variables
extern bool simulate;

#endif