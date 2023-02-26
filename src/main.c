#include "game.h"

int main(int argc, char *argv[]) {
  Game g ;//= malloc(sizeof(Game));
  /*if(g == NULL){
    printf("Game-Array could not be created !");
    return 1;
  }*/

  if(init(&g,1000,100) != 0){
    printf("Game could not be initialized !");
    return 1;
  }

  printf("hi");
  g.running = false;
  loop(&g);

  close(&g);

  //free(g);
  return 0; 
}