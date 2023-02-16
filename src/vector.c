#include "vector.h"

void ivec2_add(ivec2* a, ivec2* b, ivec2* res) {
  res->x = a->x + b->x;
  res->y = a->y + b->y;
}

void ivec2_sub(ivec2* a, ivec2* b, ivec2* res) {
  res->x = a->x - b->x;
  res->y = a->y - b->y;
}

void ivec2_scal(ivec2* a, ivec2* b, int* res) {
  *res = (a->x * b->x)+(a->y * b->y);
}

void ivec2_cross(ivec2* a, ivec2* b, int* res) {
  *res = (a->x * b->y)-(a->y * b->x);
}
