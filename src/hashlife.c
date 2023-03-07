#include "hashlife.h"
#if 0
int init_Libs(hl_globals_t *g) {
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                 SDL_GetError());
    return EXIT_FAILURE;
  }
  g->window =
      SDL_CreateWindow("Conways Game of Life", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
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
  SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);

  return EXIT_SUCCESS;
}

hl_globals_t *hl_init(int width, int height) {
  if (width % CHUNK_SIZE != 0 && height % CHUNK_SIZE != 0)
    return NULL;

  // Init globals struct
  hl_globals_t *gl = malloc(sizeof(hl_globals_t));
  memset(gl, 0, sizeof(*gl));
  return gl;
}

Uint32 hl_hash(Chunk_t *ch) {
  Uint32 h = 0;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      h += ch->cells[i][j] * (j - i + 1);
    }
  }
  return h;
}

void draw(hl_globals_t *g) {
  SDL_Surface *surf = SDL_CreateRGBSurface(0, g->dm.w, g->dm.h, 32, 0, 0, 0, 0);

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0, 0));

  // showRect = (SDL_Rect){.w = g->width * g->grid_cell_size, .h=g->height
  // *g->grid_cell_size, .x=g->offset_x, .y=g->offset_y}; SDL_Rect srcRect = {.w
  // = g->grid_width, .h=g->grid_height, .x=0, .y=0};

  SDL_LockSurface(surf);
  for (int i = 0; i < g->num_chunks; i++) {
    for (int y = 0; y < g->height; y++) {
      for (int x = 0; x < g->width; x++) {
        if (g->chunks[i]->cells[x][y]) {
          // surf_set_pixel(surf,x,y,SDL_MapRGBA(surf->format, alive_color.r,
          // alive_color.g, alive_color.b, alive_color.a));
          surf_set_pixel(surf, x, y,
                         map_2D_rainbow(surf->format, g->CELLS_SIZE, x, y));
        }
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
#endif