//
// Created by paul on 3/7/23.
//

#include "cgl.h"

// Struct for thread data
struct th_data {
  bool *cells;
  bool *cells_n;
  int f_index; // first index
  int l_index; // last index
  int arr_size;
  long grid_width;
};

void cgl_tick(cgl_data_t *g) {
  memcpy(g->cells_next, g->cells, g->arr_size);
  // copy cells to buffer array, so we don't edit the cells array during the
  // calculations

  // Multithreading
  pthread_t th_ids[NUM_THREADS];
  int step = g->arr_size / NUM_THREADS; // calculate amount of cells per thread

  // set data for threads
  struct th_data *data = (struct th_data *)malloc(sizeof(struct th_data));
  data->cells = g->cells;
  data->cells_n = g->cells_next;
  data->arr_size = g->arr_size;
  data->grid_width = g->arr_width;

  // create Threads
  int tmp_l_index = 0; // used to determine next starting position
  for (int i = 0; i < NUM_THREADS; i++) {

    data->f_index = tmp_l_index;

    if (tmp_l_index < g->arr_size) {
      data->l_index = data->f_index + step;
    } else { // number of threads is not dividable by the size of the array
      data->l_index = g->arr_size - 1; // maximum posible index of array
    }

    pthread_create(&th_ids[i], NULL, cgl_getNeighbours, data);
    pthread_join(th_ids[i], NULL);

    tmp_l_index = data->l_index;
  }

  // wait for threads to finish
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(th_ids[i], NULL);
  }

  // buffer swap
  memcpy(g->cells, g->cells_next, g->arr_size);
  g->generation++;
  free(data);
}

void *cgl_getNeighbours(void *p) {
  struct th_data *d = p;

  for (int i = d->f_index; i < d->l_index; i++) {
    int neighbours = 0;
    asm volatile goto( // This is needs less than half of the instructions the
                       // old algorithm needed.
        "pushq %%r8\n\t"
        "pushq %%rax\n\t" // push all registers we are using

        "neg:\n\t"
        "movq %[arr_p], %%r8\n\t" // get pointer to array

        //"subq $0x1,%%r8\n\t" //left cell
        "dec %%r8\n\t" // only works on one-byte arrays
        "call cop\n\t" // compare

        "addq %[grd_wdt], %%r8\n\t" // below-left cell
        "call cop\n\t"

        //"addq $0x1,%%r8\n\t" //below cell
        "inc %%r8\n\t" // only works on one-byte arrays
        "call cop\n\t"

        //"addq $0x1,%%r8\n\t" //below-right cell
        "inc %%r8\n\t" // only works on one-byte arrays
        "call cop\n\t"

        "subq %[grd_wdt], %%r8\n\t" // right cell
        "call cop\n\t"

        "subq %[grd_wdt], %%r8\n\t" // above-right cell
        "call cop\n\t"

        //"subq $0x1,%%r8\n\t" //above cell
        "dec %%r8\n\t" // only works on one-byte arrays
        "call cop\n\t"

        //"subq $0x1,%%r8\n\t" //above-left cell
        "dec %%r8\n\t" // only works on one-byte arrays
        "call cop\n\t"

        "popq %%rax\n\t" // pop all registers we are using
        "popq %%r8\n\t"

        "jmp %l[finish]\n\t"

        "incr:\n\t"
        "inc %[ngbrs]\n\t" // increment neighbours
        "ret\n\t"

        "cop:\n\t"
        "movq (%%r8), %%rax\n\t" // copy bool to register
        "test %%al, %%al\n\t" // check if 0 here, %al is the lower byte of %eax,
        // we only need to check this one.
        "jnz incr\n\t" // if not 0 (if cell alive increase neighbours), jump to
        // incr
        "ret\n\t" // else return

        : [ngbrs] "+r"(neighbours)      // output
        : [arr_p] "r"(&d->cells[i]),    // cells array pointer  input
          [grd_wdt] "ir"(d->grid_width) //(GRD_WDT) // grid width
        : "cc", "memory", "r8", "rax"   // clobber
        : finish);
  finish:
    // calculate if alive
    if (d->cells[i]) {
      if (neighbours > 3 || neighbours < 2) {
        d->cells_n[i] = 0;
      }
    } else if (neighbours == 3) {
      d->cells_n[i] = 1;
    }
  }
  pthread_exit(0);
}

cgl_data_t *alloc_CGL(void) {
  cgl_data_t *g;

  g = malloc(sizeof(cgl_data_t));
  if (g == NULL) {
    printf("Failed to initialize CGL_DATA-Array !\n");
    return NULL;
  }

  memset(g, 0, sizeof(*g));
  return g;
}

cgl_data_t *cgl_init(int cells_size) {
  cgl_data_t *g = alloc_CGL();

  g->arr_width = cells_size;
  g->arr_size = g->arr_width * g->arr_width; // size of bool = 1
  g->cells = malloc(g->arr_size * sizeof(bool));
  g->cells_next = malloc(g->arr_size * sizeof(bool));
  g->generation = 0;

  return g;
}

void cgl_free(cgl_data_t *g) {
  if (g != NULL)
    free(g);
}

void cgl_flipCell(cgl_data_t *g, int index) {
  if (index >= 0 && index < g->arr_size)
    g->cells[index] = !g->cells[index];
}

int cgl_getIndex(cgl_data_t *g, int x, int y) {
  return modulo(x + (y * g->arr_width), g->arr_size);
}

bool cgl_getCellAt(cgl_data_t *g, int x, int y) {
  return g->cells[cgl_getIndex(g, x, y)];
}

bool cgl_isAlive(bool *cells, int index, int arr_size) {
  if (index >= 0 && index < arr_size)
    return cells[index];
  return 0;
}
