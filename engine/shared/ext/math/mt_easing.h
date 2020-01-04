#ifndef mt_easing_h__
#define mt_easing_h__

#include "core/math/mt_base.h"

NAMESPACE_BEGIN(mt)
NAMESPACE_BEGIN(easing)

inline f32 smoothstep(f32 x, f32 a, f32 b)
{
  if(x < a)
    return 0.0;
  else if(x >= b)
    return 1.0;
  x = (x-a) / (b-a);
  return (x * x) * (3 - 2 * x);
}

// simple linear tweening - no easing
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration
inline f32 linearTween(f32 t, f32 b, f32 c, f32 d) {
  return c * t / d + b;
};

///////////// QUADRATIC EASING: t^2 ///////////////////

// quadratic easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration
// t and d can be in frames or seconds/milliseconds
inline f32 easeInQuad(f32 t, f32 b, f32 c, f32 d) {
  return c * (t /= d) * t + b;
};

// quadratic easing out - decelerating to zero velocity
inline f32 easeOutQuad(f32 t, f32 b, f32 c, f32 d) {
  return -c * (t /= d) * (t - 2) + b;
};

// quadratic easing in/out - acceleration until halfway, then deceleration
inline f32 easeInOutQuad(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d / 2) < 1)
    return c / 2 * t * t + b;
  return -c / 2 * ((t -= 1) * (t - 2) - 1) + b;
};

///////////// CUBIC EASING: t^3 ///////////////////////

// cubic easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration
// t and d can be frames or seconds/milliseconds
inline f32 easeInCubic(f32 t, f32 b, f32 c, f32 d) {
  return c * (t /= d) * t * t + b;
};

// cubic easing out - decelerating to zero velocity
inline f32 easeOutCubic(f32 t, f32 b, f32 c, f32 d) {
  return c * ((t = t / d - 1) * t * t + 1) + b;
};

// cubic easing in/out - acceleration until halfway, then deceleration
inline f32 easeInOutCubic(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d / 2) < 1)
    return c / 2 * t * t * t + b;
  return c / 2 * (( t -= 2) * t * t + 2) + b;
};

///////////// QUARTIC EASING: t^4 /////////////////////

// quartic easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration
// t and d can be frames or seconds/milliseconds
inline f32 easeInQuart(f32 t, f32 b, f32 c, f32 d) {
  return c * (t /= d) * t * t * t + b;
};

// quartic easing out - decelerating to zero velocity
inline f32 easeOutQuart(f32 t, f32 b, f32 c, f32 d) {
  return -c * ((t = t / d - 1) * t * t * t - 1) + b;
};

// quartic easing in/out - acceleration until halfway, then deceleration
inline f32 easeInOutQuart(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d / 2) < 1)
    return c / 2 * t * t * t * t + b;
  return -c/2 * ((t -= 2) * t * t * t - 2) + b;
};

///////////// QUINTIC EASING: t^5  ////////////////////

// quintic easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration
// t and d can be frames or seconds/milliseconds
inline f32 easeInQuint(f32 t, f32 b, f32 c, f32 d) {
  return c * (t /= d) * t * t * t * t + b;
};

// quintic easing out - decelerating to zero velocity
inline f32 easeOutQuint(f32 t, f32 b, f32 c, f32 d) {
  return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
};

// quintic easing in/out - acceleration until halfway, then deceleration
inline f32 easeInOutQuint(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d / 2) < 1)
    return c / 2 * t * t * t * t * t + b;
  return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
};

///////////// SINUSOIDAL EASING: sin(f32 t) ///////////////

// sinusoidal easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in position, f32 d: duration
inline f32 easeInSine(f32 t, f32 b, f32 c, f32 d) {
  return -c * cosf(t / d * (MF_PI / 2 )) + c + b;
};

// sinusoidal easing out - decelerating to zero velocity
inline f32 easeOutSine(f32 t, f32 b, f32 c, f32 d) {
  return c * sinf(t / d * (MF_PI / 2)) + b;
};

// sinusoidal easing in/out - accelerating until halfway, then decelerating
inline f32 easeInOutSine(f32 t, f32 b, f32 c, f32 d) {
  return -c / 2 * (cosf(MF_PI * t / d) - 1) + b;
};

///////////// EXPONENTIAL EASING: 2^t /////////////////

// exponential easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in position, f32 d: duration
inline f32 easeInExpo(f32 t, f32 b, f32 c, f32 d) {
  return (t == 0) ? b : c * powf(2, 10 * (t / d - 1)) + b;
};

// exponential easing out - decelerating to zero velocity
inline f32 easeOutExpo(f32 t, f32 b, f32 c, f32 d) {
  return (t == d) ? b + c : c * (-powf(2, -10 * t / d) + 1) + b;
};

// exponential easing in/out - accelerating until halfway, then decelerating
inline f32 easeInOutExpo(f32 t, f32 b, f32 c, f32 d) {
  if (t == 0)
    return b;
  if (t == d)
    return b + c;
  if ((t /= d / 2) < 1)
    return c/2 * powf(2, 10 * (t - 1)) + b;
  return c/2 * (-powf(2, -10 * (t -= 1)) + 2) + b;
};


/////////// CIRCULAR EASING: sqrt(1-t^2) //////////////

// circular easing in - accelerating from zero velocity
// t: current time, f32 b: beginning value, f32 c: change in position, f32 d: duration
inline f32 easeInCirc(f32 t, f32 b, f32 c, f32 d) {
  return -c * (sqrtf(1 - (t /= d) * t) - 1) + b;
};

// circular easing out - decelerating to zero velocity
inline f32 easeOutCirc(f32 t, f32 b, f32 c, f32 d) {
  return c * sqrtf(1 - (t = t / d - 1) * t) + b;
};

// circular easing in/out - acceleration until halfway, then deceleration
inline f32 easeInOutCirc(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d / 2) < 1)
    return -c / 2 * (sqrtf(1 - t * t) - 1) + b;
  return c/2 * (sqrtf(1 - (t -= 2) * t) + 1) + b;
};


/////////// ELASTIC EASING: exponentially decaying sine wave  //////////////

// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration, a: amplitude (optional), p: period (optional)
// t and d can be in frames or seconds/milliseconds

inline f32 easeInElastic(f32 t, f32 b, f32 c, f32 d, f32 a, f32 p) {
  if (t == 0)
    return b;
  if ((t /= d) == 1)
    return b + c;
  if (!p)
    p = d * 0.3f;
  const f32 s = (a < fabs(c)) ? (a = c, p / 4) : (p / (2 * MF_PI) * asinf (c / a));
  return -(a * powf(2, 10 * (t -= 1)) * sin( (t * d - s) * (2 * MF_PI) / p )) + b;
};

inline f32 easeOutElastic(f32 t, f32 b, f32 c, f32 d, f32 a, f32 p) {
  if (t == 0)
    return b;
  if ((t /= d) == 1)
    return b + c;
  if (!p)
    p = d * 0.3f;
  const f32 s = (a < fabs(c)) ? (a = c, p / 4) : (p / (2 * MF_PI) * asinf (c / a));
  return a * powf(2,-10 * t) * sin( (t * d - s) * (2 * MF_PI) / p ) + c + b;
};

inline f32 easeInOutElastic(f32 t, f32 b, f32 c, f32 d, f32 a, f32 p) {
  if (t == 0)
    return b;
  if ((t /= d / 2) == 2)
    return b + c;
  if (!p)
    p = d * (0.3f * 1.5f);
  const f32 s = (a < fabs(c)) ? ( a = c, p/4) : (p / ( 2 * MF_PI) * asinf (c / a));
  if (t < 1)
    return -0.5f * (a * powf(2, 10 * (t -= 1)) * sinf( (t * d - s) * (2 * MF_PI) / p )) + b;
  return a * powf(2, -10 * (t -= 1)) * sinf( (t * d - s) * (2 * MF_PI) / p) * 0.5f + c + b;
};


/////////// BACK EASING: overshooting cubic easing: (s+1)*t^3 - s*t^2  //////////////

// back easing in - backtracking slightly, then reversing direction and moving to target
// t: current time, f32 b: beginning value, f32 c: change in value, f32 d: duration, s: overshoot amount (optional)
// t and d can be in frames or seconds/milliseconds
// s controls the amount of overshoot: higher s means greater overshoot
// s has a default value of 1.70158, which produces an overshoot of 10 percent
// s==0 produces cubic easing with no overshoot
inline f32 easeInBack(f32 t, f32 b, f32 c, f32 d, f32 s) {
  if (s == 0.0f)
    s = 1.70158f;
  return c * (t /= d) * t * ((s + 1) * t - s) + b;
};

// back easing out - moving towards target, overshooting it slightly, then reversing and coming back to target
inline f32 easeOutBack(f32 t, f32 b, f32 c, f32 d, f32 s) {
  if (s == 0.0f)
    s = 1.70158f;
  return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
};

// back easing in/out - backtracking slightly, then reversing direction and moving to target,
// then overshooting target, reversing, and finally coming back to target
inline f32 easeInOutBack(f32 t, f32 b, f32 c, f32 d, f32 s) {
  if (s == 0.0f)
    s = 1.70158f; 
  if ((t /= d / 2) < 1)
    return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
  return c / 2 * ((t -= 2) * t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
};


/////////// BOUNCE EASING: exponentially decaying parabolic bounce  //////////////


// bounce easing out
inline f32 easeOutBounce(f32 t, f32 b, f32 c, f32 d) {
  if ((t /= d) < (1.0f / 2.75f))
    return c * (7.5625f * t * t) + b;
  else if (t < (2.0f / 2.75f))
    return c * (7.5625f * (t -= (1.5f / 2.75f)) * t + 0.75f) + b;
  else if (t < (2.5f / 2.75f))
    return c * (7.5625f * (t -= (2.25f / 2.75f)) * t + 0.9375f) + b;
  else
    return c * (7.5625f * (t -= (2.625f / 2.75f)) * t + 0.984375f) + b;
};

// bounce easing in
// t: current time, f32 b: beginning value, f32 c: change in position, f32 d: duration
inline f32 easeInBounce(f32 t, f32 b, f32 c, f32 d) {
  return c - easeOutBounce (d - t, 0.0f, c, d) + b;
};

// bounce easing in/out
inline f32 easeInOutBounce(f32 t, f32 b, f32 c, f32 d) {
  if (t < d / 2)
    return easeInBounce (t * 2, 0, c, d) * 0.5f + b;
  return easeOutBounce (t * 2 - d, 0, c, d) * 0.5f + c * 0.5f + b;
};

NAMESPACE_END(easing)
NAMESPACE_END(mt)

#endif // mt_easing_h__
