#include "hashlife.h"
hl_globals_t *hl_init(int width, int height) {
  if (width % CHUNK_SIZE != 0 && height % CHUNK_SIZE != 0)
    return NULL;

  // Init globals struct
  hl_globals_t *gl = malloc(sizeof(hl_globals_t));
  memset(gl, 0, sizeof(*gl));
  return gl;
}
#if 0
Uint32 hl_hash(Chunk_t *ch) {
  Uint32 h = 0;
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      h += ch->cells[i][j] * (j - i + 1);
    }
  }
  return h;
}
#endif