#include "game.h"

int main(int argc, char *argv[]) {
  struct Game *g;

  g = alloc_Game();

  if(g == NULL){
    printf("Game-Array could not be created !\n");
    return 1;
  }

  if(set_vals(g,1000,100) == EXIT_FAILURE){
    printf("Game Values could not be set !\n");
    return 1;
  }
  
  loop(g);

  close(g);

  return 0; 
}
