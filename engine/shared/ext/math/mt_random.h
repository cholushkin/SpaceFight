#ifndef mt_random_h__
#define mt_random_h__

#include "core/common/com_misc.h"

NAMESPACE_BEGIN(mt)

//////////////////////////////////////////////////////////////////////////
// Random

class fast_random_generator 
{
public:
  u32 rand() // u32 1..0xffffffffu
  { return (++Idx, Idx %= NucleatorSize, Noise[Idx] ^= Noise[(Idx + 3) % NucleatorSize]); }

  f32 frand() // float 0..1
  { return( f32(rand()) / f32(MAX_RAND)); }

  explicit fast_random_generator(u32 n)
    : Idx(0) { srand(n); }
  fast_random_generator()
    : Idx(0) { srand(); }

  void srand(u32);
  void srand();
  void Reset( u32 n = 0 ) 
  { Idx = 0; srand(n); }
  static const u32 MAX_RAND = 0xffffffffu;
private:
  static const u32 NucleatorSize = 64u;

  u32 Idx;
  u32 Noise[NucleatorSize];
};

NAMESPACE_END(mt)

#endif // mt_random_h__
