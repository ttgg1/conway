#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include <SDL2/SDL.h>

#include <SDL2/SDL_video.h>

#define DIV_ROUND_CLOSEST(n, d) ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d)/2)/(d)) : (((n) + (d)/2)/(d)))

#define CELLS_SIZE 1000
#define NUM_THREADS 100

// Grid init
const int grid_cell_size = 1;
const int grid_width = CELLS_SIZE;
const int grid_height = CELLS_SIZE;

// + 1 so that the last grid lines fit in the screen.
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;

// define colors
SDL_Color back_color = {22, 22, 22, 255};  // Barely Black
SDL_Color line_color = {44, 44, 44, 255};  // Dark grey
SDL_Color alive_color = {255, 255, 255, 255};  // White
SDL_Color grid_line_color = {44, 44, 44, 255};  // Dark grey

// SDL Variables
bool running = true;
SDL_Window *window;
SDL_Renderer *renderer;

// Game Variables
bool simulate = false;

const unsigned int arr_size = sizeof(bool)*CELLS_SIZE*CELLS_SIZE;

//Helpers
bool isAlive(bool *cells, int index){
  if(index >= 0 && index < CELLS_SIZE*CELLS_SIZE)
    return cells[index];
  printf("ARGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
  return 0;
}
void flipCell(bool *cells,int index){
  if(index >= 0 && index < CELLS_SIZE*CELLS_SIZE)
    cells[index] = !cells[index];
}

int modulo(int a, int b) {
  if (a >= 0) {
    return a % b;
  }
  return b+(a % b);
}

// Struct for thread data
struct th_data{
  bool* cells;
  bool* cells_n;
  int f_index; //first index
  int l_index; //last index
};

// Game functions

void *getNeighbours(void* p){
    struct th_data *d = p;
    
    for(int i = d->f_index;i<d->l_index;i++){
      int neigbours = 0;
      //calculate neigbours
      if(d->cells[modulo((i-grid_width-1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; //top left
      if(d->cells[modulo((i-grid_width+1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; //top right

      if(d->cells[modulo((i-grid_width),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // above
      if(d->cells[modulo((i+grid_width),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // below
    
      if(d->cells[modulo((i-1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // left
      if(d->cells[modulo((i+1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // right

      if(d->cells[modulo((i+grid_width-1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // bottom left
      if(d->cells[modulo((i+grid_width+1),CELLS_SIZE*CELLS_SIZE)] == 1)neigbours++; // bottom right
    
      /*
      printf("Zelle %i\n",i);
      printf("Nachbarn: %i\n",neigbours);
      printf("Top Left %i \n",modulo((i-grid_width-1),CELLS_SIZE*CELLS_SIZE));
      printf("Top Right %i \n",modulo((i-grid_width+1),CELLS_SIZE*CELLS_SIZE));
      printf("AMOGUS %i \n",modulo((i-grid_width),CELLS_SIZE*CELLS_SIZE));
      printf("below %i \n",modulo((i+grid_width),CELLS_SIZE*CELLS_SIZE));
      printf("left %i \n",modulo((i-1),CELLS_SIZE*CELLS_SIZE));
      printf("right %i \n",modulo((i+1),CELLS_SIZE*CELLS_SIZE));
      printf("bottom left %i \n",modulo((i+grid_width-1),CELLS_SIZE*CELLS_SIZE));
      printf("bottom right %i \n",modulo((i+grid_width+1),CELLS_SIZE*CELLS_SIZE));
      printf("\n\n");
      */
      //calculate if alive
      if(isAlive(d->cells,i)){
        if(neigbours > 3 || neigbours < 2) {
          d->cells_n[i] = 0;
        }
      } else if(neigbours == 3){
          d->cells_n[i] = 1;
      }
    }
    pthread_exit(0);
}

void tick(bool *cells, bool *cells_next) {
  memcpy(cells_next, cells, arr_size); // copy cells to buffer array, so we dont edit the cells array during the calculations

  // Multithreading for ULTIMATE SPEEEEEEEEDDDDDDD
  pthread_t th_ids[NUM_THREADS];
  int step = CELLS_SIZE*CELLS_SIZE / NUM_THREADS; // calculate amount of cells per thread
  // TODO:dynamically set number of threads
  
  // set data for threads
  struct th_data *data = (struct th_data*) malloc(sizeof(struct th_data));
  data->cells = cells;
  data->cells_n = cells_next;

  // create Threads
  int tmp_l_index = 0; // used to determine next starting position
  for(int i=0;i<NUM_THREADS;i++){

    data->f_index=tmp_l_index;

    if(tmp_l_index < CELLS_SIZE*CELLS_SIZE){
      data->l_index=data->f_index+step;
    } else { // number of threads is not dividable by the size of the array
      data->l_index=CELLS_SIZE*CELLS_SIZE-1;// maximum posible index of array
    }

    pthread_create(&th_ids[i],NULL,getNeighbours,data);
    pthread_join(th_ids[i],NULL);

    tmp_l_index=data->l_index;
  }

  // wait for threads to finish
  for(int i = 0;i<NUM_THREADS;i++){
    pthread_join(th_ids[i],NULL);
  }

  // bufferswap
  memcpy(cells, cells_next, arr_size);
  free(data);
}

void draw(bool *cells) {  
  // Draw game
  SDL_SetRenderDrawColor(renderer, back_color.r,back_color.g,back_color.b,back_color.a);
  SDL_RenderClear(renderer);
  // Rect for cell drawing
  SDL_Rect cell = {0,0,grid_cell_size,grid_cell_size};

  for(int i=0;i<CELLS_SIZE*CELLS_SIZE;i++) {
    if(!isAlive(cells,i)){
      // Move rect
      cell.x = (int)(i%grid_width)*grid_cell_size;
      cell.y = (int)floor(i/grid_width)*grid_cell_size;
      //change color
      SDL_SetRenderDrawColor(renderer, grid_line_color.r,grid_line_color.g,grid_line_color.b,grid_line_color.a);
      //draw rect
      SDL_RenderDrawRect(renderer, &cell);
    } else {
      // Move rect
      cell.x = (int)(i%grid_width)*grid_cell_size;
      cell.y = (int)floor(i/grid_width)*grid_cell_size;
      //change color
      SDL_SetRenderDrawColor(renderer, alive_color.r,alive_color.g,alive_color.b,alive_color.a);
      //draw rect
      SDL_RenderFillRect(renderer, &cell);  
    }
  }
  SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
  double frameStart = 0;
  double dtime = 0;
  float fps = 0;
  //create dynamicly allocated Array.
  bool *g_cells = malloc(arr_size);
  bool *g_cells_next = malloc(arr_size);

  for (int i = 0; i < CELLS_SIZE*CELLS_SIZE-1; i++) {
      // Acess Array using pointer iteration
      g_cells[i] = 0;
      g_cells_next[i] = 0;
  }

  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                   SDL_GetError());
      return EXIT_FAILURE;
  }
  if (SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window,
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
            case SDLK_SPACE:
              tick(g_cells, g_cells_next);
              draw(g_cells);
              break;
            case SDLK_s:
              for (int i = 0; i < CELLS_SIZE*CELLS_SIZE-1; i++) {
                  g_cells[i] = rand() & 1;
              }
              break;
          }

        case SDL_MOUSEBUTTONDOWN:
          int index = (int) e.motion.y / grid_cell_size * grid_width + e.motion.x / grid_cell_size;
          flipCell(g_cells, index);
          break;
        case SDL_QUIT:
          running = false;
          simulate = false;
          break;
      }
    }
    if(simulate) {
      tick(g_cells, g_cells_next);
    }
    draw(g_cells);
    }

  free(g_cells);
  free(g_cells_next);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
