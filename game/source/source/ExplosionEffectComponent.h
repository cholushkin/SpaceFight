#ifndef _EXPLOSION_EFFECT_COMPONENT_H
#define _EXPLOSION_EFFECT_COMPONENT_H
#include "core/math/mt_base.h"

struct ExplosionEffectComponent final
{
    ExplosionEffectComponent(const mt::v2f& pos);
    float m_progress;
    mt::v2f m_pos;

    void UpdateProgress(float dt);
};

#endif