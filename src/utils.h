//
// Created by paul on 2/28/23.
//
#ifndef CONWAY_UTILS_H
#define CONWAY_UTILS_H
#include <SDL2/SDL.h>

// Divide and round to closest
#define DIV_ROUND_CLOSEST(n, d)                                                \
  ((((n) < 0) ^ ((d) < 0)) ? (((n) - (d) / 2) / (d)) : (((n) + (d) / 2) / (d)))

void surf_set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 map_2D_rainbow(SDL_PixelFormat *p_form, int grid_size, int x, int y);

int modulo(int a, int b);
#endif // CONWAY_UTILS_H
