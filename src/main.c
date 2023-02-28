#include "game.h"

/*
 * First Argument: Number of cells (this is getting squared, because of the square grid)
 * Second Argument: Number of threads to use (default: 10)
 * Third Argument: set 1 to use random seed
 * Fourth Argument: Number of generations to simulate
 */

int main(int argc, char *argv[]) {
    int threads = 10;
    int cells = 1000;
    int gens = 0;
    bool random_fill = false;

    if (argc >= 1) {
        cells = atoi(argv[1]);
    }
    if(argc >= 2) {
        threads = atoi(argv[2]);
    }
    if (argc >= 3) {
        random_fill = atoi(argv[3]);
    }
    if (argc >= 4) {
        gens = atoi(argv[4]);
    }

    struct Game *g = g_start(cells, threads);

    if (g == NULL) {
        printf("Failed to initialize Game !\n");
        return 1;
    }
    // Tests
    if(random_fill)
        fillRandom(g);
    if(argc >= 4) {
        for (int i = 0; i < gens; i++) {
            tick(g);
        }
        drawToText(g, "output.txt");
        return 0;
    }

    while(g->running){
      loop(g);
    }

    g_close(g);

    return 0;
}
