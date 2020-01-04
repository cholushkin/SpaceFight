#include "./r_tex_webp.h"
#include "./webp/webp/decode.h"
#include <stdlib.h>

extern "C" unsigned char* webp_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *bpp)
{
  *bpp = 4;
  return WebPDecodeRGBA(buffer, len, x, y);
}

extern "C" void webp_free(unsigned char* pb)
{
  free(pb);
}
