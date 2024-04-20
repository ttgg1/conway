//
// Created by paul on 3/7/23.
//

#ifndef CONWAY_CDL_H
#define CONWAY_CDL_H

#define NUM_THREADS 10

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

typedef struct cgl_data {
  int arr_size;
  int arr_width;
  int generation;

  bool *cells;
  bool *cells_next;
} cgl_data_t;

// playing-field is cubical
cgl_data_t *cgl_init(int cells_size);

// Game Logic
void *cgl_getNeighbours(void *p);
void cgl_tick(cgl_data_t *g);

// helpers
void cgl_flipCell(cgl_data_t *g, int index);
int cgl_getIndex(cgl_data_t *g, int x, int y);
bool cgl_getCellAt(cgl_data_t *g, int x, int y);
bool cgl_isAlive(bool *cells, int index, int arr_size);

void cgl_free(cgl_data_t *g);

#endif // CONWAY_CDL_H
