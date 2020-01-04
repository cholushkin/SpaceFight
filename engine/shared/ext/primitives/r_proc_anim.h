#ifndef r_proc_anim_h 
#define r_proc_anim_h 

#include "core/render/r_render.h"
#include "core/math/mt_base.h"
#include "ext/math/mt_colors.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"

NAMESPACE_BEGIN(r) // move to 'ani'

//////////////////////////////////////////////////////////////////////////
// Animation

struct SpriteState
{
  SpriteState()
    : m_frame(0)
    , m_angle(0.0f)
    , m_scale(1.0f, 1.0f)
    , m_alpha(1.0f)
    , m_color(mt::COLOR_WHITE){}
  u32     m_frame;
  mt::v2f m_pos;
  f32     m_angle;
  mt::v2f m_scale;
  u32     m_color;
  f32     m_alpha;
};

template<class SpriteT>
class AnimatedSprite
{
public:
  AnimatedSprite()
    : m_time(0.0f)
    , m_spr(NULL){}
  AnimatedSprite(const Sprite* spr)
    : m_time(0.0f)
    , m_spr(spr){}
  void Draw(Render& r)
  {
    if(m_spr)
      m_spr->Draw(r, m_state.m_frame, m_state.m_pos, mt::make_color(m_state.m_alpha, m_state.m_color), m_state.m_scale, m_state.m_angle, true);
  }
  void Draw(Render& r, const mt::Matrix23f& m)
  {
    if(!m_spr)
      return;
    mt::Matrix23f sm;
    mt::Matrix23f::ComposePRS(sm, m_state.m_pos, m_state.m_angle, m_state.m_scale);
    m_spr->Draw(r, m_state.m_frame, m * sm, mt::make_color(m_state.m_alpha, m_state.m_color));
  }
  f32            m_time;
  SpriteState    m_state;
  const SpriteT* m_spr;
};

//////////////////////////////////////////////////////////////////////////
// binders

template<class BaseSprite, class ModA>
class AnimatedSprite1 : public BaseSprite // $$ make base template
{
public:
  void Update(f32 dt)
  {
    if(!BaseSprite::m_spr)
      return;
    BaseSprite::m_time += dt;
    m_a.Update(BaseSprite::m_state, *BaseSprite::m_spr, BaseSprite::m_time, dt);
  }
  ModA  m_a;
};

template<class BaseSprite, class ModA, class ModB>
class AnimatedSprite2 : public BaseSprite
{
public:
  void Update(f32 dt)
  {
    if(!BaseSprite::m_spr)
      return;
    BaseSprite::m_time += dt;
    m_a.Update(BaseSprite::m_state, *BaseSprite::m_spr, BaseSprite::m_time, dt);
    m_b.Update(BaseSprite::m_state, *BaseSprite::m_spr, BaseSprite::m_time, dt);
  }
  ModA  m_a;
  ModB  m_b;
};

//////////////////////////////////////////////////////////////////////////
// mods

template<class P, class T, T P::*val, class ModA, class ModB>
class CombinerMod2
{
public:
  template<class U>
  void Update(P& state, const U&, f32 time, f32 dt)
  {
    m_a.Update(state.*val, 1, time, dt);
    m_b.Update(state.*val, 1, time, dt);
  }
  ModA  m_a;
  ModB  m_b;
};

template<class T, class V, V T::*val>
class ValueGrowMod
{
  V m_speed; // @sec
public:
  ValueGrowMod()
    : m_speed(0.0f){}
  ValueGrowMod(f32 s)
    : m_speed(s){}

  template<class U>
  void Update(T& state, const U&, f32 /*time*/, f32 dt)
  {
    state.*val += m_speed * dt;
  }
};

template<class T, class V, V T::*val>
class ValueCycleMod
{
  f32 m_time; // @sec
  V m_min;
  V m_max;
public:
  ValueCycleMod()
    : m_time(0.0f){}
  ValueCycleMod(V min, V max, f32 time)
    : m_min (min)
    , m_max (max)
    , m_time(time)
    {}

  template<class U>
  void Update(T& state, const U&, f32 time, f32 /*dt*/)
  {
    state.*val = (V)((m_max - m_min) * (fmod(time, m_time) / m_time) + m_min);
  }
};

template<class T, class V, V T::*val>
class ValuePingPongMod
{
  f32 m_time; // @sec
  V m_min;
  V m_max;
public:
  ValuePingPongMod()
    : m_time(0.0f){}
  ValuePingPongMod(V min, V max, f32 time)
    : m_min (min)
    , m_max (max)
    , m_time(time)
  {}

  template<class U>
  void Update(T& state, const U&, f32 time, f32 /*dt*/)
  {
    const f32 rem = fmod(time, m_time);
    const f32 htime = m_time * 0.5f;
    if(rem > htime)
      state.*val = (V)((m_max - m_min) * (1.0f - (rem - htime) / m_time) + m_min);
    else
      state.*val = (V)((m_max - m_min) * (rem / m_time + m_min));
  }
};


template<class T, class V, V T::*val>
class ValueSineMod
{
  V m_min;
  V m_max;
  f32 m_tmult;
public:
  ValueSineMod()
    : m_tmult((f32)M_PI){}
  ValueSineMod(V min, V max, f32 cycle_time_sec)
    : m_min(min)
    , m_max(max)
    , m_tmult((f32)M_2PI / cycle_time_sec){}

  template<class U>
  void Update(T& state, const U&, f32 time, f32 /*dt*/)
  {
    state.*val = m_min + (m_max - m_min) * (0.5f * (1.0f + mt::sin_low(time * m_tmult)));
  }
};

typedef ValueSineMod    <SpriteState, mt::v2f, &SpriteState::m_scale> ScaleSineMod;
typedef ValueGrowMod    <SpriteState, f32,     &SpriteState::m_angle> RotateMod;
typedef ValueSineMod    <SpriteState, mt::v2f, &SpriteState::m_pos  > PosSineMod;
typedef ValueCycleMod   <SpriteState, u32,     &SpriteState::m_frame> FrameCycleMod;
typedef ValuePingPongMod<SpriteState, u32,     &SpriteState::m_frame> FramePingPongMod;

inline u32 CycleFrames(f32 time, f32 fps, u32 nframes)
{
  return (u32)fmodf(time * fps, (f32)nframes);
}

NAMESPACE_END(r)

#endif // r_proc_anim_h 
