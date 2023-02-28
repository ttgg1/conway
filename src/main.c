#include "game.h"

int main(int argc, char *argv[]) {
  struct Game *g = g_start(1000,10);

  if(g == NULL){
      printf("Failed to initialize Game !\n");
      return 1;
  }

  while(g->running){
      loop(g);
  }

  g_close(g);

  return 0; 
}
