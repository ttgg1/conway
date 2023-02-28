#include "game.h"

// Globals

// define colors
SDL_Color back_color = {.r=22, .g=22, .b=22, .a=255};      // Barely Black
SDL_Color alive_color = {.r=255, .g=255, .b=255, .a=255};  // White
SDL_Color grid_line_color = {.r=44, .g=44, .b=44, .a=255}; // Dark grey

struct Game* alloc_Game(void){
  struct Game* g;

  g = malloc(sizeof(struct Game));
  if(g == NULL){
    printf("Failed to initialize Game-Array !\n");
    return NULL;
  }

  memset(g,0,sizeof(*g));
  return g;
}

void set_Game(struct Game *g,int num_cells,int num_threads){
  //init everything to standard
  g->NUM_THREADS = num_threads;
  g->CELLS_SIZE = num_cells;

  g->arr_size = g->CELLS_SIZE*g->CELLS_SIZE; //size of bool = 1
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

  // fill cells arrays
  /*for (int i = 0; i < g->arr_size - 1; i++) {
    g->cells[i] = 0;
    g->cells_next[i] = 0;
  }*/
  
  /*memset(g->cells,0,g->arr_size); //Redundant
  memset(g->cells_next,0,g->arr_size);*/
}

int init_Libs(struct Game* g){
    // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                SDL_GetError());
    return EXIT_FAILURE;
  }
  g->window = SDL_CreateWindow("Conways Game of Life",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,g->window_width,g->window_height,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  g->renderer = SDL_CreateRenderer(g->window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(g->window == NULL){
      printf("SDL Window could not be created ! \n");
      return EXIT_FAILURE;
  }
  if(g->renderer == NULL){
      printf("SDL Renderer could not be created ! \n");
      return EXIT_FAILURE;
  }
  SDL_SetRenderDrawColor( g->renderer, 0x00, 0x00, 0x00, 0xFF );

  return EXIT_SUCCESS;
}

// TODO: rewrite
void draw(struct Game *g) {
  /* Draw game
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
  SDL_RenderPresent(g->renderer);*/


    SDL_Surface *surf = SDL_CreateRGBSurface(0,g->dm.w,g->dm.h,32,0,0,0,0);
    // TODO: make ram effient, by using list of cells that are alive
    // TODO: use dynamic list
    SDL_Rect *alive_cells=malloc(g->arr_size* sizeof(SDL_Rect)); // a bit memory inefficient, but prettier
    SDL_Rect *dead_cells=malloc(g->arr_size* sizeof(SDL_Rect));

    int width_cells = DIV_ROUND_CLOSEST(g->dm.w,g->grid_cell_size); // How many cells fit into one "row"
    int height_cells = DIV_ROUND_CLOSEST(g->dm.h,g->grid_cell_size); // How many cells fit in one "collum"

   // SDL_Rect showRect = {.w = width_cells,.h=height_cells,.x=g->offset_x,.y=g->offset_y};
    SDL_Rect showRect = {.w = g->grid_width*g->grid_cell_size,.h=g->grid_height*g->grid_cell_size,.x=g->offset_x,.y=g->offset_y};
    SDL_Rect srcRect = {.w = g->grid_width,.h=g->grid_height,.x=0,.y=0};

    int num_alive = 0;
    int num_dead = 0;

    // Fill Rect array with all drawable positions.
    //TODO: optimize (use one loop)
    for(int y = 0; y < g->grid_height; y++) {
        for(int x = 0; x < g->grid_width; x++) {
            if(getCellAt(g,x,y)){
                // cell alive
                alive_cells[num_alive] = (SDL_Rect){.w = 1,.h=1,.x=x, .y=y};
                ++num_alive;
            } else {
                // cell dead
                dead_cells[num_dead] = (SDL_Rect){.w = 1,.h=1,.x=x, .y=y};
                ++num_dead;
            }
        }
    }
    //draw rects on surface
    SDL_LockSurface(surf);
    SDL_FillRects(surf,alive_cells,num_alive, SDL_MapRGBA(surf->format,alive_color.r,alive_color.g,alive_color.b,alive_color.a));
    SDL_FillRects(surf,dead_cells,num_dead, SDL_MapRGBA(surf->format,grid_line_color.r,grid_line_color.g,grid_line_color.b,grid_line_color.a));
    SDL_UnlockSurface(surf);

    // Create one big texture, that gets transformed when zooming or moving
    // Everything gets "rendered" this way, but drawing a texture is very very fast because of hardware acceleration
    SDL_Texture* tex = SDL_CreateTextureFromSurface(g->renderer,surf);

    SDL_RenderClear(g->renderer);

    SDL_RenderCopy(g->renderer,tex,&srcRect,&showRect); // TODO: use offset to only draw a small portion of the Texture

    SDL_RenderPresent(g->renderer);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
    free(alive_cells);
    free(dead_cells);
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
    int neighbours = 0;
    // calculate neighbours
    if (d->cells[modulo((i - d->grid_width - 1), d->arr_size)] == 1)
      neighbours++; // top left
    if (d->cells[modulo((i - d->grid_width + 1), d->arr_size)] == 1)
      neighbours++; // top right

    if (d->cells[modulo((i - d->grid_width), d->arr_size)] == 1)
      neighbours++; // above
    if (d->cells[modulo((i + d->grid_width), d->arr_size)] == 1)
      neighbours++; // below

    if (d->cells[modulo((i - 1), d->arr_size)] == 1)
      neighbours++; // left
    if (d->cells[modulo((i + 1), d->arr_size)] == 1)
      neighbours++; // right

    if (d->cells[modulo((i + d->grid_width - 1), d->arr_size)] == 1)
      neighbours++; // bottom left
    if (d->cells[modulo((i + d->grid_width + 1), d->arr_size)] == 1)
      neighbours++; // bottom right
 
    // calculate if alive
    if (isAlive_thread(d->cells, i, d->arr_size)) {
      if (neighbours > 3 || neighbours < 2) {
        d->cells_n[i] = 0;
      }
    } else if (neighbours == 3) {
      d->cells_n[i] = 1;
    }
  }
  pthread_exit(0);
}

void tick(struct Game *g) {
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

  // buffer swap
  memcpy(g->cells, g->cells_next, g->arr_size);
  free(data);
}

struct Game* g_start(int cells,int num_threads){
    struct Game *g;
    g = alloc_Game();

    set_Game(g,cells,num_threads);
    if(init_Libs(g) == EXIT_FAILURE){
        printf("Failed to initialize SDL !\n");
        return NULL;
    }

    return g;
}

void g_close(struct Game *g){
  free(g->cells);
  free(g->cells_next);

  SDL_DestroyRenderer(g->renderer);
  SDL_DestroyWindow(g->window);

  free(g);
  SDL_Quit();
}

void loop(struct Game *g){
    SDL_Event e;

    static int mouse_at_R_click_x = 0;
    static int mouse_at_R_click_y = 0;

    int scroll_scale = 5;
    static bool isDragged = false;
    //handle Events
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
          case SDL_KEYDOWN:
              handle_KeyEvents(g,&e);
              break;

          case SDL_MOUSEBUTTONDOWN:
            if(e.button.button == SDL_BUTTON_LEFT){ //Flip a cell if clicked
              int index = modulo(floor(e.motion.x/g->grid_cell_size) - floor(g->offset_x/g->grid_cell_size)  + ((floor(e.motion.y / g->grid_cell_size) - floor(g->offset_y/g->grid_cell_size)) * g->grid_width), g->arr_size);
              flipCell(g, index);
              //flipCell(g, getIndex(g,e.motion.x,e.motion.y));
              draw(g);
            }
            if(e.button.button == SDL_BUTTON_RIGHT && !isDragged){
                mouse_at_R_click_x = e.motion.x;
                mouse_at_R_click_y = e.motion.y;
                isDragged = true;
            }
            break;

          case SDL_MOUSEBUTTONUP:
              if(e.button.button == SDL_BUTTON_RIGHT){
                  g->offset_x += (int) (e.motion.x-mouse_at_R_click_x)/scroll_scale;
                  g->offset_y += (int) (e.motion.y-mouse_at_R_click_y)/scroll_scale;

                  mouse_at_R_click_x = mouse_at_R_click_y = 0;
                  isDragged = false;
              }
              break;

          case SDL_WINDOWEVENT:
              handle_WindowEvents(g,&e);
              break;

          case SDL_MOUSEWHEEL:
              handle_MouseEvents(g,&e);
              break;

          case SDL_QUIT:
            g->running = false;
            g->simulate = false;
            break;
          }
    }

    // Calling neesary game functions
    if (g->simulate) {
      tick(g);
    }
    draw(g);
}


bool isAlive(struct Game *g, int index) {
    return isAlive_thread(g->cells,index,g->arr_size);
}

bool isAlive_thread(bool* cells, int index, int arr_size) {
  if (index >= 0 && index < arr_size)
    return cells[index];
  printf("ARGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
  return 0;
}

void flipCell(struct Game *g, int index) {
  if (index >= 0 && index < g->arr_size)
    g->cells[index] = !g->cells[index];
}

int getIndex(struct Game *g,int x,int y){
  return modulo(x + (y*g->grid_width), g->arr_size);
}

bool getCellAt(struct Game *g,int x,int y){
  return g->cells[getIndex(g,x,y)];
}

int modulo(int a, int b){
  if (a >= 0) {
    return a % b;
  }
  return b + (a % b);
}

void handle_KeyEvents(struct Game *g, SDL_Event *e) {
    int step = 5;
    switch ((*e).key.keysym.sym) {
            // Keypresses
            case SDLK_r: // pause/play the simulation
              g->simulate = !g->simulate;
              break;
            case SDLK_SPACE: // advance one tick
              tick(g);
              draw(g);
              break;
            case SDLK_s:
              for (int i = 0; i < g->arr_size - 1; i++) { // Fill screen randomly for testing
                g->cells[i] = rand() & 1;
              }
              break;
              // Moving the plane with the arrow keys
            case SDLK_RIGHT:
              g->offset_x-=step;
              break;
            case SDLK_LEFT:
              g->offset_x+=step;
              break;
            case SDLK_UP:
              g->offset_y+=step;
              break;
            case SDLK_DOWN:
              g->offset_y-=step;
              break;
          }
}

void handle_MouseEvents(struct Game *g, SDL_Event *e) {
        if ((*e).wheel.y > 0){ // scroll up
          if(g->grid_cell_size >= g->dm.w){ // if cell-size doesnt fit on screen
            g->grid_cell_size = g->dm.w;
            return;
          }
          g->grid_cell_size += 1;
          return;
        }

        if ((*e).wheel.y < 0){ // scroll down
          g->grid_cell_size -= 1;

          if(g->grid_cell_size <= 0){ // if cell-size goes negative it will be set to the minimum value
            g->grid_cell_size = 1;
            return;
          }
        }
}

void handle_WindowEvents(struct Game *g, SDL_Event *e) {
    if((*e).window.event == SDL_WINDOWEVENT_RESIZED || (*e).window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
          SDL_GetCurrentDisplayMode(0,&g->dm); // get new window size
          draw(g); // redraw the screen after window resize
    }
}
