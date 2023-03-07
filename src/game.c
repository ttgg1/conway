#include "game.h"

// Globals

// define colors
SDL_Color alive_color = {.r = 255, .g = 255, .b = 255, .a = 255};  // White
SDL_Color grid_line_color = {.r = 44, .g = 44, .b = 44, .a = 255}; // Dark grey
SDL_Rect showRect = {0, 0, 0, 0};

int mouse_x;
int mouse_y;

cgl_data_t *data;

struct Game *alloc_Game(void) {
  struct Game *g;

  g = malloc(sizeof(struct Game));
  if (g == NULL) {
    printf("Failed to initialize Game-Array !\n");
    return NULL;
  }

  memset(g, 0, sizeof(*g));
  return g;
}

void g_set_Game(struct Game *g, int num_cells) {
  // init everything to standard
  g->CELLS_SIZE = num_cells;

  data = cgl_init(num_cells);

  g->grid_cell_size = 1;
  g->offset_x = 0;
  g->offset_y = 0;

  g->simulate = false;
  g->running = true;
}

int g_init_Libs(struct Game *g) {
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }
  g->window = SDL_CreateWindow("Conways Game of Life", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, 1000, 1000,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  g->renderer = SDL_CreateRenderer(
      g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (g->window == NULL) {
    printf("SDL Window could not be created ! \n");
    return EXIT_FAILURE;
  }
  if (g->renderer == NULL) {
    printf("SDL Renderer could not be created ! \n");
    return EXIT_FAILURE;
  }
  SDL_SetRenderDrawColor(g->renderer, grid_line_color.r, grid_line_color.g,
                         grid_line_color.b, grid_line_color.a);
  SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);

  return EXIT_SUCCESS;
}

void g_draw(struct Game *g) {
  SDL_Surface *surf = SDL_CreateRGBSurface(0, g->dm.w, g->dm.h, 32, 0, 0, 0, 0);

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0, 0));

  showRect = (SDL_Rect){.w = g->CELLS_SIZE * g->grid_cell_size,
                        .h = g->CELLS_SIZE * g->grid_cell_size,
                        .x = g->offset_x,
                        .y = g->offset_y};
  SDL_Rect srcRect = {.w = g->CELLS_SIZE, .h = g->CELLS_SIZE, .x = 0, .y = 0};

  SDL_LockSurface(surf);

  for (int y = 0; y < g->CELLS_SIZE; y++) {
    for (int x = 0; x < g->CELLS_SIZE; x++) {
      if (cgl_getCellAt(data, x, y)) {
        // surf_set_pixel(surf,x,y,SDL_MapRGBA(surf->format, alive_color.r,
        // alive_color.g, alive_color.b, alive_color.a));
        surf_set_pixel(surf, x, y,
                       map_2D_rainbow(surf->format, g->CELLS_SIZE, x, y));
      }
    }
  }
  SDL_UnlockSurface(surf);

  // Create one big texture, that gets transformed when zooming or moving
  // Everything gets "rendered" this way, but drawing a texture is very very
  // fast because of hardware acceleration Also this means smooth zooming and
  // moving
  SDL_Texture *tex = SDL_CreateTextureFromSurface(g->renderer, surf);

  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

  SDL_RenderClear(g->renderer);

  SDL_RenderCopy(g->renderer, tex, &srcRect, &showRect);

  SDL_RenderPresent(g->renderer);

  SDL_FreeSurface(surf);
  SDL_DestroyTexture(tex);
}

struct Game *g_start(int cells) {
  struct Game *g;
  g = alloc_Game();

  g_set_Game(g, cells);
  if (g_init_Libs(g) == EXIT_FAILURE) {
    printf("Failed to initialize SDL !\n");
    return NULL;
  }

  return g;
}

void g_close(struct Game *g) {
  cgl_free(data);

  SDL_DestroyRenderer(g->renderer);
  SDL_DestroyWindow(g->window);

  free(g);
  SDL_Quit();
}

void handle_KeyEvents(struct Game *g, SDL_Event *e) {
  int step = 10;
  switch ((*e).key.keysym.sym) {
  // Keypresses
  case SDLK_r: // pause/play the simulation
    g->simulate = !g->simulate;
    break;
  case SDLK_SPACE: // advance one cgl_tick
    cgl_tick(data);
    g_draw(g);
    break;
  case SDLK_s:
    g_fillRandom();
    break;
    // Moving the plane with the arrow keys
  case SDLK_RIGHT:
    g->offset_x -= step;
    break;
  case SDLK_LEFT:
    g->offset_x += step;
    break;
  case SDLK_UP:
    g->offset_y += step;
    break;
  case SDLK_DOWN:
    g->offset_y -= step;
    break;
  case SDLK_t:
    g->offset_x = 0;
    g->offset_y = 0;
    g->grid_cell_size = 1;
    break;
  case SDLK_PAGEUP:
    ++g->grid_cell_size;
    break;
  case SDLK_PAGEDOWN:
    --g->grid_cell_size;
    break;
  }
}

void handle_MouseEvents(struct Game *g, SDL_Event *e) {
  if ((*e).wheel.y > 0) {               // scroll up
    if (g->grid_cell_size >= g->dm.w) { // if cell-size doesnt fit on screen
      g->grid_cell_size = g->dm.w;
      return;
    }
    g->grid_cell_size += 1;
    return;
  }

  if ((*e).wheel.y < 0) { // scroll down
    g->grid_cell_size -= 1;

    if (g->grid_cell_size <=
        0) { // if cell-size goes negative it will be set to the minimum value
      g->grid_cell_size = 1;
      return;
    }
  }
}

void handle_WindowEvents(struct Game *g, SDL_Event *e) {
  if ((*e).window.event == SDL_WINDOWEVENT_RESIZED ||
      (*e).window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    SDL_GetCurrentDisplayMode(0, &g->dm); // get new window size
    g_draw(g); // redraw the screen after window resize
  }
}

void g_loop(struct Game *g) {
  SDL_Event e;

  static int mouse_at_R_click_x = 0;
  static int mouse_at_R_click_y = 0;

  static bool isDragged = false;
  // handle Events
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_KEYDOWN:
      handle_KeyEvents(g, &e);
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (e.button.button == SDL_BUTTON_LEFT) { // Flip a cell if clicked
        int index = modulo(floor(e.motion.x / g->grid_cell_size) -
                               floor(g->offset_x / g->grid_cell_size) +
                               ((floor(e.motion.y / g->grid_cell_size) -
                                 floor(g->offset_y / g->grid_cell_size)) *
                                g->CELLS_SIZE),
                           data->arr_size);
        cgl_flipCell(data, index);
        // flipCell(g, getIndex(g,e.motion.x,e.motion.y));
        g_draw(g);
      }
      if (e.button.button == SDL_BUTTON_RIGHT && !isDragged) {
        mouse_at_R_click_x = e.motion.x;
        mouse_at_R_click_y = e.motion.y;
        isDragged = true;
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (e.button.button == SDL_BUTTON_RIGHT) {
        mouse_at_R_click_x = mouse_at_R_click_y = 0;
        isDragged = false;
      }
      break;

    case SDL_MOUSEMOTION:
      if (isDragged) {
        g->offset_x += e.motion.xrel;
        g->offset_y += e.motion.yrel;

        SDL_GetMouseState(&mouse_x, &mouse_y);
      }

    case SDL_WINDOWEVENT:
      handle_WindowEvents(g, &e);
      break;

    case SDL_MOUSEWHEEL:
      handle_MouseEvents(g, &e);
      break;

    case SDL_QUIT:
      g->running = false;
      g->simulate = false;
      break;
    }
  }

  // Calling neesary game functions
  if (g->simulate) {
    cgl_tick(data);
  }
  g_draw(g);
}

void g_drawToText(struct Game *g, char *filename) {
  FILE *f = fopen(filename, "a");

  int lines = 1;

  if (f == NULL) {
    printf("Error opening file!\n");
    exit(0);
  }

  for (int i = 0; i < data->arr_size; i++) {
    if (i == g->CELLS_SIZE * lines) {
      fprintf(f, "\n");
      ++lines;
    }
    fprintf(f, "%c", data->cells[i] ? '#' : '.');
  }
  fprintf(f, "\n\n\n");
  fclose(f);
}

void g_fillRandom() {
  for (int i = 0; i < data->arr_size - 1;
       i++) { // Fill screen randomly for testing
    data->cells[i] = rand() & 1;
  }
}

cgl_data_t *g_getData() { return data; }
void g_sim_gens_to_txt(int cells_size, int gens, char *file, bool fillRandom) {
  struct Game *g;
  g = alloc_Game();
  g_set_Game(g, cells_size);

  if (fillRandom)
    g_fillRandom();

  for (int i = 0; i < gens; i++) {
    cgl_tick(g_getData());
  }
  g_drawToText(g, "output.txt");
}
