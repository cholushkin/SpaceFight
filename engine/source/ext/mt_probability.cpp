#include "ext/mathalgs/mt_probability.h"

NAMESPACE_BEGIN(mt)

fast_random_generator ProbabilityHelper::s_rnd;

bool ProbabilityHelper::TrySpawnEvent( const f32 probability ) 
{
  EASSERT( probability >= 0.0f )
  EASSERT( probability <= 1.0f );
  return s_rnd.frand() <= probability;
}

i16 ProbabilityHelper::SpawnEvent( const f32* probs, const u32 sz)
{
  EASSERT(0!=sz);
  EASSERT(NULL!=probs);
  // get prob line
  f32 sum = 0;
  for(u32 i = 0; i < sz; ++i) 
  {
    EASSERT(probs[i] >= 0.0f);
    sum += probs[i];
  }

  // select val
  f32 point = s_rnd.frand() * sum;

  // return event
  for(u16 i = 0; i < sz; ++i)
    if( (point -= probs[i]) < 0 )
      return (i16)i;

  return -1;
}

bool ProbabilityHelper::Coin()
{
  return 0 == (s_rnd.rand() % 2);
}

f32 ProbabilityHelper::Rand( const RangeF& diap )
{
  return diap.x + s_rnd.frand() * (diap.y - diap.x);
}

i16 ProbabilityHelper::Rand( const RangeI16& diap )
{
  return diap.x + s_rnd.rand() % (diap.y - diap.x + 1);
}

NAMESPACE_END(mt)