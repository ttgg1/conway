#include "game.h"

void draw() {
  // Draw game
  SDL_SetRenderDrawColor(renderer, back_color.r, back_color.g, back_color.b,
                         back_color.a);
  SDL_RenderClear(renderer);
  // Rect for cell drawing
  SDL_Rect cell = {0, 0, grid_cell_size, grid_cell_size};

  int width_cells = DIV_ROUND_CLOSEST(dm.w,grid_cell_size); // How many cells fit into one "row"
  int height_cells = DIV_ROUND_CLOSEST(dm.h,grid_cell_size); // How many cells fit in one "collum"

  for(int y = offset_y; y < offset_y + height_cells; y++) {
    for(int x = offset_x; x < offset_x + width_cells; x++) {
      cell.x = (x-offset_x) * grid_cell_size;
      cell.y = (y-offset_y) * grid_cell_size;
      if (!getCellAt(g_cells, x, y)) {
        // change color
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                             grid_line_color.b, grid_line_color.a);
        // draw rect
        SDL_RenderDrawRect(renderer, &cell);

      } else {
        SDL_SetRenderDrawColor(renderer, alive_color.r, alive_color.g,
                             alive_color.b, alive_color.a);
        // draw rect
        SDL_RenderFillRect(renderer, &cell);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

// Struct for thread data
struct th_data {
  bool *cells;
  bool *cells_n;
  int f_index; // first index
  int l_index; // last index
};

void *getNeighbours(void *p) {
  struct th_data *d = p;

  for (int i = d->f_index; i < d->l_index; i++) {
    int neigbours = 0;
    // calculate neigbours
    if (d->cells[modulo((i - grid_width - 1), arr_size)] == 1)
      neigbours++; // top left
    if (d->cells[modulo((i - grid_width + 1), arr_size)] == 1)
      neigbours++; // top right

    if (d->cells[modulo((i - grid_width), arr_size)] == 1)
      neigbours++; // above
    if (d->cells[modulo((i + grid_width), arr_size)] == 1)
      neigbours++; // below

    if (d->cells[modulo((i - 1), arr_size)] == 1)
      neigbours++; // left
    if (d->cells[modulo((i + 1), arr_size)] == 1)
      neigbours++; // right

    if (d->cells[modulo((i + grid_width - 1), arr_size)] == 1)
      neigbours++; // bottom left
    if (d->cells[modulo((i + grid_width + 1), arr_size)] == 1)
      neigbours++; // bottom right
 
    // calculate if alive
    if (isAlive(d->cells, i)) {
      if (neigbours > 3 || neigbours < 2) {
        d->cells_n[i] = 0;
      }
    } else if (neigbours == 3) {
      d->cells_n[i] = 1;
    }
  }
  pthread_exit(0);
}

void tick() {
  memcpy(g_cells_next, g_cells,
         arr_size); // copy cells to buffer array, so we dont edit the cells
                    // array during the calculations

  // Multithreading for ULTIMATE SPEEEEEEEEDDDDDDD
  pthread_t th_ids[NUM_THREADS];
  int step = arr_size /
             NUM_THREADS; // calculate amount of cells per thread
  // TODO:dynamically set number of threads

  // set data for threads
  struct th_data *data = (struct th_data *)malloc(sizeof(struct th_data));
  data->cells = g_cells;
  data->cells_n = g_cells_next;

  // create Threads
  int tmp_l_index = 0; // used to determine next starting position
  for (int i = 0; i < NUM_THREADS; i++) {

    data->f_index = tmp_l_index;

    if (tmp_l_index < arr_size) {
      data->l_index = data->f_index + step;
    } else { // number of threads is not dividable by the size of the array
      data->l_index =
          arr_size - 1; // maximum posible index of array
    }

    pthread_create(&th_ids[i], NULL, getNeighbours, data);
    pthread_join(th_ids[i], NULL);

    tmp_l_index = data->l_index;
  }

  // wait for threads to finish
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(th_ids[i], NULL);
  }

  // bufferswap
  memcpy(g_cells, g_cells_next, arr_size);
  free(data);
}

int init(const int num_cells, const int num_threads){
  CELLS_SIZE = num_cells;
  NUM_THREADS = num_threads;
  // enough, because sizeof(bool) = 1
  arr_size = CELLS_SIZE * CELLS_SIZE;

  // legacy
  grid_width = CELLS_SIZE;
  grid_height = CELLS_SIZE;
  window_width = grid_width + 1;
  window_height = grid_height + 1;

  // Grid init
  grid_cell_size = 1;
  offset_x = 0;
  offset_y = 0;

  // define colors
  back_color = (SDL_Color){22, 22, 22, 255};      // Barely Black
  line_color = (SDL_Color){44, 44, 44, 255};      // Dark grey
  alive_color = (SDL_Color){255, 255, 255, 255};  // White
  grid_line_color = (SDL_Color){44, 44, 44, 255}; // Dark grey

  simulate = false;
  running = true;

  //init cells arrays
  g_cells = malloc(arr_size*sizeof(bool));
  g_cells_next = malloc(arr_size*sizeof(bool));

  // fill cellsssss arrays snake
  for (int i = 0; i < arr_size - 1; i++) {
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
  if (SDL_CreateWindowAndRenderer(window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                  &renderer) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s",
                  SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_SetWindowTitle(window, "Conways Game of Life");
  return 0;
}

void close(){
  running = false;

  free(g_cells);
  free(g_cells_next);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void loop(){
    while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        // Keypresses
        case SDLK_r:
          simulate = !simulate;
          break;
        case SDLK_SPACE:
          tick(g_cells, g_cells_next);
          draw(g_cells);
          break;
        case SDLK_s:
          for (int i = 0; i < arr_size - 1; i++) {
            g_cells[i] = rand() & 1;
          }
          break;
        case SDLK_RIGHT:
          offset_x--;
          break;
        case SDLK_LEFT:
          offset_x++;
          break;
        case SDLK_UP:
          offset_y++;
          break;
        case SDLK_DOWN:
          offset_y--;
          break;
       
      }
      case SDL_MOUSEBUTTONDOWN:
        if(e.button.button == SDL_BUTTON_LEFT){
          int index = modulo(floor(e.motion.x/grid_cell_size) + offset_x  + ((floor(e.motion.y / grid_cell_size) + offset_y) * grid_width), arr_size);
          flipCell(g_cells, index);
        } 
        break;
        

      case SDL_WINDOWEVENT:
        if(e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
          SDL_GetCurrentDisplayMode(0,&dm);
          draw(g_cells);
        }
        break;

      case SDL_MOUSEWHEEL:
        if (e.wheel.y > 0){ // scroll up
          if(grid_cell_size >= dm.w){
            grid_cell_size = dm.w;
            break;
          }
          
          grid_cell_size += 1;
          break;
        }

        if (e.wheel.y < 0){ // scroll down 
          grid_cell_size -= 1;

          if(grid_cell_size <= 0){
            grid_cell_size = 1;
            break;
          }
        }
        break;

      case SDL_QUIT:
        running = false;
        simulate = false;
        break;
      }
    }
    if (simulate) {
      tick(g_cells, g_cells_next);
    }
    draw(g_cells);
  }
}

bool isAlive(bool *cells, int index) {
  if (index >= 0 && index < arr_size)
    return cells[index];
  printf("ARGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
  return 0;
}

void flipCell(bool *cells, int index) {
  if (index >= 0 && index < arr_size)
    cells[index] = !cells[index];
}

int getIndex(int x,int y){
  return modulo(x + (y*grid_width), arr_size);
}

bool getCellAt(bool* cells,int x,int y){
  return cells[getIndex(x,y)];
}

int modulo(int a, int b){
  if (a >= 0) {
    return a % b;
  }
  return b + (a % b);
}