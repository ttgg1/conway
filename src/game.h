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
struct Game{
  // Cells Arrays
  int CELLS_SIZE;
  int NUM_THREADS;
  bool *cells;
  bool *cells_next;
  int arr_size;

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
};

// Init
void set_Game(struct Game* g,int num_cells,int num_threads);
int init_Libs(struct Game* g);
struct Game* alloc_Game(void);

struct Game* g_start(int cells,int num_threads);
void g_close(struct Game *g);

// Game Logic
void *getNeighbours(void* p); // executed using threads
void tick(struct Game *g);
//execute this function while Game.running is true
void loop(struct Game *g);

void handle_KeyEvents(struct Game *g, SDL_Event *e);
void handle_MouseEvents(struct Game *g, SDL_Event *e);
void handle_WindowEvents(struct Game *g, SDL_Event *e);

// Output
void draw(struct Game *g);

// Helper Functions
bool isAlive(struct Game *g, int index);
bool isAlive_thread(bool* cells, int index, int arr_size); //overload for internal use
void flipCell(struct Game *g, int index);
int getIndex(struct Game *g,int x,int y);
bool getCellAt(struct Game *g,int x,int y);

int modulo(int a, int b);

// define colors
extern SDL_Color back_color;
extern SDL_Color alive_color;
extern SDL_Color grid_line_color;

#endif