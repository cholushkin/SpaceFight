#ifndef prob_probability_h__
#define prob_probability_h__

#include "core/math/mt_base.h"
#include "core/common/com_misc.h"
#include "ext/math/mt_random.h"

NAMESPACE_BEGIN(mt)

typedef mt::v2f   RangeF;
typedef mt::v2i16 RangeI16;

class ProbabilityHelper 
{
public:
  static bool TrySpawnEvent(const f32 probability);
  static i16  SpawnEvent(const f32* probs, const u32 sz);

  static bool Coin();
  static f32  Rand(const RangeF&   diap);
  static i16  Rand(const RangeI16& diap);

private:
  static fast_random_generator s_rnd;
};

NAMESPACE_END(mt)
#endif // prob_probability_h__
