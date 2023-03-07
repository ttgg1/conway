#ifndef _HASHLIFE_H_
#define _HASHLIFE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define CHUNK_SIZE 5 //-> 25 bit needed
#define WIN_W 1000
#define WIN_H 1000

typedef struct {
  // create 2d-bit array
  bool cells[CHUNK_SIZE * CHUNK_SIZE];
} Chunk_t;

typedef struct {
  int width;
  int height;
  int num_chunks;
  Chunk_t *chunks;

  SDL_Window *window;
  SDL_Renderer *renderer;
} hl_globals_t;

Uint32 hl_hash(Chunk_t *ch);

void hl_fill_random(hl_globals_t *gl);

// width and height need to be dividable by the chunk size (5)
hl_globals_t *hl_init(int width, int height);

#endif