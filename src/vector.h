#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct ivec2 {
  int x;
  int y;
} ivec2;

void ivec2_add(ivec2* a, ivec2* b, ivec2* res);
void ivec2_sub(ivec2* a, ivec2* b, ivec2* res);

void ivec2_scal(ivec2* a, ivec2* b, int* res);
void ivec2_cross(ivec2* a, ivec2* b, int* res);  // equal to determinate of both inputs

#endif
