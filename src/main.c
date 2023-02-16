#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "vector.h"


#define CELLS_SIZE 255

// Grid init
const int grid_cell_size = 36;
const int grid_width = CELLS_SIZE;
const int grid_height = CELLS_SIZE;

// + 1 so that the last grid lines fit in the screen.
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;

// Dark theme.
SDL_Color grid_background = {22, 22, 22, 255};  // Barely Black
SDL_Color grid_line_color = {44, 44, 44, 255};  // Dark grey
SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
SDL_Color grid_cursor_color = {255, 255, 255, 255};  // White

// SDL Variables
bool running = true;
SDL_Window *window;
SDL_Renderer *renderer;

// Game Variables
bool simulate = false;

// Game functions
void tick(bool (*cells)[CELLS_SIZE][CELLS_SIZE]) {
  for (int i = 0; i < CELLS_SIZE-1; i++) {
    for (int j = 0; j < CELLS_SIZE-1; j++) {
      int neigbours = 0;
      //calculate neigbours
      if((*cells)[i-1][j+1] == 1)neigbours++; //top left
      if((*cells)[i+1][j+1] == 1)neigbours++; //top right

      if((*cells)[i][j+1] == 1)neigbours++; // above
      if((*cells)[i][j-1] == 1)neigbours++; // below
    
      if((*cells)[i-1][j] == 1)neigbours++; // left
      if((*cells)[i+1][j] == 1)neigbours++; // right

      if((*cells)[i-1][j-1] == 1)neigbours++; // bottom left
      if((*cells)[i+1][j-1] == 1)neigbours++; // bottom right
    
      //calculate if alive
      if((*cells)[i][j] == 1){
        if(neigbours > 3 || neigbours < 2) {
          (*cells)[i][j] = 0;
        }
        else if(neigbours == 3){
          (*cells)[i][j] = 1;
        }
      }
    }
  }
}

void draw(bool (*cells)[CELLS_SIZE][CELLS_SIZE]) {
  // Draw grid background.
  SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g,
                         grid_background.b, grid_background.a);
  SDL_RenderClear(renderer);

  // Draw grid lines.
  SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                         grid_line_color.b, grid_line_color.a);

  for (int x = 0; x < 1 + grid_width * grid_cell_size;
       x += grid_cell_size) {
      SDL_RenderDrawLine(renderer, x, 0, x, window_height);
  }

  for (int y = 0; y < 1 + grid_height * grid_cell_size;
       y += grid_cell_size) {
      SDL_RenderDrawLine(renderer, 0, y, window_width, y);
  }
}

int main(void) {
  //create dynamic 2d Array.
  bool (*g_cells)[CELLS_SIZE][CELLS_SIZE] = malloc(sizeof *g_cells);

  for (int i = 0; i < CELLS_SIZE; i++) {
    for (int j = 0; j < CELLS_SIZE; j++) {
      // Acess 2D-Array using pointer iteration
      (*g_cells)[i][j] = 0;
    }
  }

  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                   SDL_GetError());
      return EXIT_FAILURE;
  }
  if (SDL_CreateWindowAndRenderer(window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                  &renderer) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Create window and renderer: %s", SDL_GetError());
      return EXIT_FAILURE;
  }

  SDL_SetWindowTitle(window, "Conways Game of Life");

  while(running) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_KEYDOWN:
          switch(e.key.keysym.sym) {
            //Keypresses
            case SDLK_r:
              simulate = !simulate;
              break;
          }

        case SDL_MOUSEBUTTONDOWN:
          (*g_cells)[(int) e.motion.x / grid_cell_size][(int) e.motion.y / grid_cell_size] = 1;
          break;
        case SDL_QUIT:
          running = false;
          simulate = false;
          break;
      }
    }
    tick(g_cells);
    draw(g_cells);
  }

  free(g_cells);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
