#include "ExplosionEffectComponent.h"

ExplosionEffectComponent::ExplosionEffectComponent(const mt::v2f& pos)
    : m_progress(0.0f)
    , m_pos(pos)
{
}

void ExplosionEffectComponent::UpdateProgress(float dt)
{
    m_progress = mt::Clamp(m_progress + dt, 0.0f, 1.0f);
}