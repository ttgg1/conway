#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "vector.h"


#define CELLS_SIZE 255

// Grid init
const int grid_cell_size = 5;
const int grid_width = CELLS_SIZE;
const int grid_height = CELLS_SIZE;

// + 1 so that the last grid lines fit in the screen.
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;

// define colors
SDL_Color back_color = {22, 22, 22, 255};  // Barely Black
SDL_Color line_color = {44, 44, 44, 255};  // Dark grey
SDL_Color alive_color = {255, 255, 255, 255};  // White

// SDL Variables
bool running = true;
SDL_Window *window;
SDL_Renderer *renderer;

// Game Variables
bool simulate = false;

//Helpers
bool isAlive(bool (*cells)[CELLS_SIZE][CELLS_SIZE], int x,int y){
  return (*cells)[x][y]; 
}
void flipCell(bool (*cells)[CELLS_SIZE][CELLS_SIZE], int x,int y){
  (*cells)[x][y] = !(*cells)[x][y];
}

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
  // Draw game
  
  // Rect for cell drawing
  SDL_Rect cell = {0,0,grid_cell_size,grid_cell_size};

  for(int i=0;i<CELLS_SIZE;i++) {
    for(int j=0;j<CELLS_SIZE;j++) {
      if(!isAlive(cells,i,j)){
        // Move rect
        cell.x = (int)i*grid_cell_size;
        cell.y = (int)j*grid_cell_size;
        //change color
        SDL_SetRenderDrawColor(renderer, back_color.r,back_color.g,back_color.b,back_color.a);
        //draw rect
        SDL_RenderDrawRect(renderer, &cell);
      } else if (isAlive(cells,i,j)) {
        // Move rect
        cell.x = (int)i*grid_cell_size;
        cell.y = (int)j*grid_cell_size;
        //change color
        SDL_SetRenderDrawColor(renderer, alive_color.r,alive_color.g,alive_color.b,alive_color.a);
        //draw rect
        SDL_RenderFillRect(renderer, &cell);
      }
    }
  }
  SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
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
          flipCell(g_cells, ((int) e.motion.x * grid_cell_size), ((int) e.motion.y * grid_cell_size));
          //(*g_cells)[(int) e.motion.x * grid_cell_size][(int) e.motion.y * grid_cell_size] = !(*g_cells)[(int) e.motion.x * grid_cell_size][(int) e.motion.y * grid_cell_size];
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
