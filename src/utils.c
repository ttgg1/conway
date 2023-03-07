#include "utils.h"

void surf_set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
  Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
  *(Uint32 *)target_pixel = pixel;
}

Uint32 map_2D_rainbow(SDL_PixelFormat *p_form, int grid_size, int x, int y) {
  int counts = DIV_ROUND_CLOSEST(grid_size, 255);
  int r = (x / counts) % 255;
  int g = (y / counts) % 255;
  int b = (-y / counts) % 255;
  return SDL_MapRGBA(p_form, r, g, b, 255);
}

int modulo(int a, int b) {
  if (a >= 0) {
    return a % b;
  }
  return b + (a % b);
}