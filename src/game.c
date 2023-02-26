#include "game.h"

// Globals

// define colors
SDL_Color back_color = (SDL_Color){22, 22, 22, 255};      // Barely Black
SDL_Color line_color = (SDL_Color){44, 44, 44, 255};      // Dark grey
SDL_Color alive_color = (SDL_Color){255, 255, 255, 255};  // White
SDL_Color grid_line_color = (SDL_Color){44, 44, 44, 255}; // Dark grey

int init(Game* g,const int num_cells, const int num_threads){
  printf("hi init 1");
  //init everything to standard
  g->NUM_THREADS = num_threads;
  g->CELLS_SIZE = num_cells;

  g->arr_size= g->CELLS_SIZE*g->CELLS_SIZE;

  g->cells = malloc(g->arr_size*sizeof(bool));
  g->cells_next = malloc(g->arr_size*sizeof(bool));

  g->grid_cell_size = 1;
  g->offset_x = 0;
  g->offset_y = 0;

  g->simulate = false;
  g->running = true;

  g->grid_width = g->CELLS_SIZE;
  g->grid_height = g->CELLS_SIZE;
  g->window_width = g->grid_width + 1;
  g->window_height = g->grid_height + 1;
  
  printf("hi init 2");

  // fill cells arrays
  for (int i = 0; i < g->arr_size - 1; i++) {
    g->cells[i] = 0;
    g->cells_next[i] = 0;
  }
  
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                SDL_GetError());
    return EXIT_FAILURE;
  }
  if (SDL_CreateWindowAndRenderer(g->window_width, g->window_height, SDL_WINDOW_RESIZABLE, &g->window,
                                  &g->renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s",
                  SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_SetWindowTitle(g->window, "Conways Game of Life");
  return 0;
}

void draw(Game *g) {
  // Draw game
  SDL_SetRenderDrawColor(g->renderer, back_color.r, back_color.g, back_color.b,
                         back_color.a);
  SDL_RenderClear(g->renderer);
  // Rect for cell drawing
  SDL_Rect cell = {0, 0, g->grid_cell_size, g->grid_cell_size};

  int width_cells = DIV_ROUND_CLOSEST(g->dm.w,g->grid_cell_size); // How many cells fit into one "row"
  int height_cells = DIV_ROUND_CLOSEST(g->dm.h,g->grid_cell_size); // How many cells fit in one "collum"

  for(int y = g->offset_y; y < g->offset_y + height_cells; y++) {
    for(int x = g->offset_x; x < g->offset_x + width_cells; x++) {
      cell.x = (x - g->offset_x) * g->grid_cell_size;
      cell.y = (y - g->offset_y) * g->grid_cell_size;
      if (!getCellAt(g, x, y)) {
        // change color
        SDL_SetRenderDrawColor(g->renderer, grid_line_color.r, grid_line_color.g,
                             grid_line_color.b, grid_line_color.a);
        // draw rect
        SDL_RenderDrawRect(g->renderer, &cell);

      } else {
        SDL_SetRenderDrawColor(g->renderer, alive_color.r, alive_color.g,
                             alive_color.b, alive_color.a);
        // draw rect
        SDL_RenderFillRect(g->renderer, &cell);
      }
    }
  }

  SDL_RenderPresent(g->renderer);
}

// Struct for thread data
// This struct can be skipped, but passing the entire game struct may be a bit overkill
// also the game struct doesnt contain f_index and l_index;
struct th_data {
  bool *cells;
  bool *cells_n;
  int f_index; // first index
  int l_index; // last index
  int arr_size;
  int grid_width;
};

void *getNeighbours(void *p) {
  struct th_data *d = p;

  for (int i = d->f_index; i < d->l_index; i++) {
    int neigbours = 0;
    // calculate neigbours
    if (d->cells[modulo((i - d->grid_width - 1), d->arr_size)] == 1)
      neigbours++; // top left
    if (d->cells[modulo((i - d->grid_width + 1), d->arr_size)] == 1)
      neigbours++; // top right

    if (d->cells[modulo((i - d->grid_width), d->arr_size)] == 1)
      neigbours++; // above
    if (d->cells[modulo((i + d->grid_width), d->arr_size)] == 1)
      neigbours++; // below

    if (d->cells[modulo((i - 1), d->arr_size)] == 1)
      neigbours++; // left
    if (d->cells[modulo((i + 1), d->arr_size)] == 1)
      neigbours++; // right

    if (d->cells[modulo((i + d->grid_width - 1), d->arr_size)] == 1)
      neigbours++; // bottom left
    if (d->cells[modulo((i + d->grid_width + 1), d->arr_size)] == 1)
      neigbours++; // bottom right
 
    // calculate if alive
    if (isAlive_thread(d->cells, i, d->arr_size)) {
      if (neigbours > 3 || neigbours < 2) {
        d->cells_n[i] = 0;
      }
    } else if (neigbours == 3) {
      d->cells_n[i] = 1;
    }
  }
  pthread_exit(0);
}

void tick(Game *g) {
  memcpy(g->cells_next, g->cells,g->arr_size); 
  // copy cells to buffer array, so we dont edit the cells array during the calculations

  // Multithreading for ULTIMATE SPEEEEEEEEDDDDDDD
  pthread_t th_ids[g->NUM_THREADS];
  int step = g->arr_size /
             g->NUM_THREADS; // calculate amount of cells per thread
  // TODO:dynamically set number of threads

  // set data for threads
  struct th_data *data = (struct th_data *)malloc(sizeof(struct th_data));
  data->cells = g->cells;
  data->cells_n = g->cells_next;
  data->arr_size = g->arr_size;
  data->grid_width = g->grid_width;

  // create Threads
  int tmp_l_index = 0; // used to determine next starting position
  for (int i = 0; i < g->NUM_THREADS; i++) {

    data->f_index = tmp_l_index;

    if (tmp_l_index < g->arr_size) {
      data->l_index = data->f_index + step;
    } else { // number of threads is not dividable by the size of the array
      data->l_index =
          g->arr_size - 1; // maximum posible index of array
    }

    pthread_create(&th_ids[i], NULL, getNeighbours, data);
    pthread_join(th_ids[i], NULL);

    tmp_l_index = data->l_index;
  }

  // wait for threads to finish
  for (int i = 0; i < g->NUM_THREADS; i++) {
    pthread_join(th_ids[i], NULL);
  }

  // bufferswap
  memcpy(g->cells, g->cells_next, g->arr_size);
  free(data);
}

void close(Game *g){
  g->running = false;

  free(g->cells);
  free(g->cells_next);

  SDL_DestroyRenderer(g->renderer);
  SDL_DestroyWindow(g->window);
  SDL_Quit();
}

void loop(Game *g){
    while (g->running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        // Keypresses
        case SDLK_r:
          g->simulate = !g->simulate;
          break;
        case SDLK_SPACE:
          tick(g);
          draw(g);
          break;
        case SDLK_s:
          for (int i = 0; i < g->arr_size - 1; i++) {
            g->cells[i] = rand() & 1;
          }
          break;
        case SDLK_RIGHT:
          g->offset_x--;
          break;
        case SDLK_LEFT:
          g->offset_x++;
          break;
        case SDLK_UP:
          g->offset_y++;
          break;
        case SDLK_DOWN:
          g->offset_y--;
          break;
       
      }
      case SDL_MOUSEBUTTONDOWN:
        if(e.button.button == SDL_BUTTON_LEFT){
          int index = modulo(floor(e.motion.x/g->grid_cell_size) + g->offset_x  + ((floor(e.motion.y / g->grid_cell_size) + g->offset_y) * g->grid_width), g->arr_size);
          flipCell(g, index);
        } 
        break;
        

      case SDL_WINDOWEVENT:
        if(e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
          SDL_GetCurrentDisplayMode(0,&g->dm);
          draw(g);
        }
        break;

      case SDL_MOUSEWHEEL:
        if (e.wheel.y > 0){ // scroll up
          if(g->grid_cell_size >= g->dm.w){
            g->grid_cell_size = g->dm.w;
            break;
          }
          
          g->grid_cell_size += 1;
          break;
        }

        if (e.wheel.y < 0){ // scroll down 
          g->grid_cell_size -= 1;

          if(g->grid_cell_size <= 0){
            g->grid_cell_size = 1;
            break;
          }
        }
        break;

      case SDL_QUIT:
        g->running = false;
        g->simulate = false;
        break;
      }
    }
    if (g->simulate) {
      tick(g);
    }
    draw(g);
  }
}

bool isAlive(Game *g, int index) {
  if (index >= 0 && index < g->arr_size)
    return g->cells[index];
  printf("ARGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
  return 0;
}

bool isAlive_thread(bool* cells, int index, int arr_size) {
  if (index >= 0 && index < arr_size)
    return cells[index];
  printf("ARGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
  return 0;
}

void flipCell(Game *g, int index) {
  if (index >= 0 && index < g->arr_size)
    g->cells[index] = !g->cells[index];
}

int getIndex(Game *g,int x,int y){
  return modulo(x + (y*g->grid_width), g->arr_size);
}

bool getCellAt(Game *g,int x,int y){
  return g->cells[getIndex(g,x,y)];
}

int modulo(int a, int b){
  if (a >= 0) {
    return a % b;
  }
  return b + (a % b);
}