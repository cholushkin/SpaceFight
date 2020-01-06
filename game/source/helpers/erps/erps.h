#ifndef erps_h__
#define erps_h__

#include "core/common/com_misc.h"
#include "core/math/mt_base.h"

NAMESPACE_BEGIN(erps)

enum EnErpName
{
  ERP_ACCELERATE_DECELERATE       = 0,
  ERP_ACCELERATE                  = 1,
  ERP_ACCELERATE1                 = 2,
  ERP_ACCELERATE2                 = 3,
  ERP_ACCELERATE3                 = 4,
 	ERP_ANTICIPATE                  = 5,
  ERP_ANTICIPATE1                 = 6,
  ERP_ANTICIPATE2                 = 7,
  ERP_ANTICIPATE3                 = 8,
  ERP_ANTICIPATE_OVERSHOOT        = 9,
  ERP_ANTICIPATE_OVERSHOOT1       = 10,
  ERP_ANTICIPATE_OVERSHOOT2       = 11,
  ERP_ANTICIPATE_OVERSHOOT3       = 12,
  ERP_BOUNCE                      = 13,
  ERP_CYCLE                       = 14,
  ERP_DECELERATE                  = 15,
  ERP_DECELERATE1                 = 16,
  ERP_DECELERATE2                 = 17,
  ERP_DECELERATE3                 = 18,
  ERP_OVERSHOOT                   = 19,
  ERP_OVERSHOOT1                  = 20,
  ERP_OVERSHOOT2                  = 21,
  ERP_OVERSHOOT3                  = 22,
  ERP_LINEAR                      = 23,
  ERP_COUNT                       = 24
};

typedef f32 (*ErpFunctionPointer)(f32 p1);
ErpFunctionPointer GetErp(EnErpName erpName);


// ----- erp functions
f32 ErpAccelerateDecelerate(f32 input);
f32 ErpAccelerate(f32 input); // default one
f32 ErpAccelerate1(f32 input);
f32 ErpAccelerate2(f32 input);
f32 ErpAccelerate3(f32 input);
f32 ErpLinear(f32 input);
f32 ErpAnticipate(f32 input);
f32 ErpAnticipate1(f32 input);
f32 ErpAnticipate2(f32 input);
f32 ErpAnticipate3(f32 input);
f32 ErpAnticipateOvershoot(f32 input);
f32 ErpAnticipateOvershoot1(f32 input);
f32 ErpAnticipateOvershoot2(f32 input);
f32 ErpAnticipateOvershoot3(f32 input);
f32 ErpBounce(f32 input);
f32 ErpCycle(f32 input);
f32 ErpDecelerate(f32 input);
f32 ErpDecelerate1(f32 input);
f32 ErpDecelerate2(f32 input);
f32 ErpDecelerate3(f32 input);
f32 ErpOvershoot(f32 input);
f32 ErpOvershoot1(f32 input);
f32 ErpOvershoot2(f32 input);
f32 ErpOvershoot3(f32 input);



NAMESPACE_END(erps)


#endif // erps_h__