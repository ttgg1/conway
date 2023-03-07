#include "game.h"

/*
 * First Argument: Number of cells (this is getting squared, because of the
 * square grid) Second Argument: set 1 to use random seed Third Argument: Number
 * of generations to simulate (0 for normal mode)
 *
 */

int main(int argc, char *argv[]) {
  int cells = 1000;
  int gens = 0;
  bool random_fill = false;

  if (argc > 1) {
    cells = atoi(argv[1]);
    printf("Cells: %i\n", cells);
  }
  if (argc > 2) {
    random_fill = atoi(argv[2]);
    printf("Random Fill: %i\n", random_fill);
  }
  if (argc > 3) {
    gens = atoi(argv[3]);
    printf("Generations: %i\n", gens);
  }
  struct Game *g;

  if (gens > 0) {
    g_sim_gens_to_txt(cells,gens,"output.txt",random_fill);
  } else {
    g = g_start(cells);

    if (g == NULL) {
      printf("Failed to initialize Game !\n");
      return 1;
    }
  }

  if (random_fill)
    g_fillRandom();


  while (g->running) {
    g_loop(g);
  }

  g_close(g);

  return 0;
}
