#include "PhysicsObstacleComponent.h"

using namespace vp;

PhysicsObstacleComponent::PhysicsObstacleComponent(vp::Obstacle* obstacle)
    : m_obstacle(obstacle)
{
    EASSERT(obstacle != nullptr);
}
