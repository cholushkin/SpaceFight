#include "ext/math/mt_random.h"
#include "core/math/mt_base.h"

NAMESPACE_BEGIN(mt)

//////////////////////////////////////////////////////////////////////////
// Random

void fast_random_generator::srand(u32 n)
{
  static const u32 StartNoise = 0x6E9B2271;
  ::srand(n);
  for(n = 0u; n < NucleatorSize; ++n)
  {
    Noise[n]=StartNoise;
    for(u32 j = 0u; j < 8u*sizeof(*Noise); j += 11u)
      Noise[n] ^= ::rand() << j;
  }
  Idx = 0;
}

void fast_random_generator::srand()
{
  for(u32 n = 0u; n < NucleatorSize; ++n)
    for(u32 j = 0u; j < 8u * sizeof(*Noise); j += 11u)
      Noise[n] ^= ::rand() << j;
  Idx = 0;
}

NAMESPACE_END(mt)