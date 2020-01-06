#include "erps.h"

NAMESPACE_BEGIN(erps)


ErpFunctionPointer GetErp( EnErpName erpName )
{
  switch(erpName)
  {
  case ERP_ACCELERATE_DECELERATE:
    return ErpAccelerateDecelerate;
  case ERP_ACCELERATE:
    return ErpAccelerate;
  case ERP_ACCELERATE1:
    return ErpAccelerate1;
  case ERP_ACCELERATE2:
    return ErpAccelerate2;
  case ERP_ACCELERATE3:
    return ErpAccelerate3;
  case ERP_ANTICIPATE:
    return ErpAnticipate;
  case ERP_ANTICIPATE1:
    return ErpAnticipate1;
  case ERP_ANTICIPATE2:
    return ErpAnticipate2;
  case ERP_ANTICIPATE3:
    return ErpAnticipate3;
  case ERP_ANTICIPATE_OVERSHOOT:
    return ErpAnticipateOvershoot;
  case ERP_ANTICIPATE_OVERSHOOT1:
    return ErpAnticipateOvershoot1;
  case ERP_ANTICIPATE_OVERSHOOT2:
    return ErpAnticipateOvershoot2;
  case ERP_ANTICIPATE_OVERSHOOT3:
    return ErpAnticipateOvershoot3;
  case ERP_BOUNCE:
    return ErpBounce;
  case ERP_CYCLE:
    return ErpCycle;
  case ERP_DECELERATE:
    return ErpDecelerate;
  case ERP_DECELERATE1:
    return ErpDecelerate1;
  case ERP_DECELERATE2:
    return ErpDecelerate2;
  case ERP_DECELERATE3:
    return ErpDecelerate3;
  case ERP_OVERSHOOT:
    return ErpOvershoot;
  case ERP_OVERSHOOT1:
    return ErpOvershoot1;
  case ERP_OVERSHOOT2:
    return ErpOvershoot2;
  case ERP_OVERSHOOT3:
    return ErpOvershoot3;
  case ERP_LINEAR:
    return ErpLinear;
  default:
    EALWAYS_ASSERT("");
  }
  return NULL;
}

// ----------------------------------------------------------------------------
f32 ErpAccelerateDecelerate( f32 input )
{
  return (f32)(cosf((input + 1.0f) * (f32)M_PI) / 2.0f) + 0.5f;
}

f32 ErpAccelerate( f32 input )
{
  return input * input;
}

f32 ErpAccelerate1( f32 input )
{
  return pow(input,1.5f);
}

f32 ErpAccelerate2( f32 input )
{
  return pow(input,2.5f);
}

f32 ErpAccelerate3( f32 input )
{
  return input * input * input;
}

f32 ErpAnticipate( f32 input )
{
  return input * input * ((0.6f + 1) * input - 0.6f);
}

f32 ErpAnticipate1( f32 input )
{
  return input * input * ((1.2f + 1) * input - 1.2f);
}

f32 ErpAnticipate2( f32 input )
{
  return input * input * ((1.6f + 1) * input - 1.6f);
}

f32 ErpAnticipate3( f32 input )
{
  return input * input * ((2.3f + 1) * input - 2.3f);
}


f32 a(f32 t, f32 s) {
  return t * t * ((s + 1) * t - s);
}
f32 o(f32 t, f32 s) {
  return t * t * ((s + 1) * t + s);
}

f32 ErpAnticipateOvershoot( f32 input )
{
  static const f32 s_tension = 0.6f;
  if (input < 0.5f) 
    return 0.5f * a(input * 2.0f, s_tension);
  else 
    return 0.5f * (o(input * 2.0f - 2.0f, s_tension) + 2.0f);
}

f32 ErpAnticipateOvershoot1( f32 input )
{
  static const f32 s_tension = 1.2f;
  if (input < 0.5f) 
    return 0.5f * a(input * 2.0f, s_tension);
  else 
    return 0.5f * (o(input * 2.0f - 2.0f, s_tension) + 2.0f);
}

f32 ErpAnticipateOvershoot2( f32 input )
{
  static const f32 s_tension = 2.3f;
  if (input < 0.5f) 
    return 0.5f * a(input * 2.0f, s_tension);
  else 
    return 0.5f * (o(input * 2.0f - 2.0f, s_tension) + 2.0f);
}

f32 ErpAnticipateOvershoot3( f32 input )
{
  static const f32 s_tension = 4.8f;
  if (input < 0.5f) 
    return 0.5f * a(input * 2.0f, s_tension);
  else 
    return 0.5f * (o(input * 2.0f - 2.0f, s_tension) + 2.0f);
}

f32 bounce(f32 t) {
  return t * t * 8.0f;
}

f32 ErpBounce( f32 input )
{
  input *= 1.1226f;
  if (input < 0.3535f) 
    return bounce(input);
  else if (input < 0.7408f) 
    return bounce(input - 0.54719f) + 0.7f;
  else if (input < 0.9644f) 
    return bounce(input - 0.8526f) + 0.9f;
  else 
    return bounce(input - 1.0435f) + 0.95f;
}

f32 ErpCycle( f32 input )
{
  return (f32)(sinf( (f32)M_PI * input));
}

f32 ErpDecelerate( f32 input )
{
  return (f32)(1.0f - (1.0f - input) * (1.0f - input));
}

f32 ErpDecelerate1( f32 input )
{
  return (f32)(1.0f - pow((1.0f - input), 2 * 1.1f));
}

f32 ErpDecelerate2( f32 input )
{
  return (f32)(1.0f - pow((1.0f - input), 2 * 1.5f));
}

f32 ErpDecelerate3( f32 input )
{
  return (f32)(1.0f - pow((1.0f - input), 2 * 2.0f));
}

f32 ErpOvershoot( f32 input )
{
  input -= 1.0f;
  return input * input * ((0.6f + 1) * input + 0.6f) + 1.0f;
}

f32 ErpOvershoot1( f32 input )
{
  input -= 1.0f;
  return input * input * ((1.2f + 1) * input + 1.2f) + 1.0f;
}

f32 ErpOvershoot2( f32 input )
{
  input -= 1.0f;
  return input * input * ((2.3f + 1) * input + 2.3f) + 1.0f;
}

f32 ErpOvershoot3( f32 input )
{
  input -= 1.0f;
  return input * input * ((3.6f + 1) * input + 3.6f) + 1.0f;
}

f32 ErpLinear( f32 input )
{
  return input;
}



NAMESPACE_END(erps)
