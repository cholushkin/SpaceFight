#ifndef _PHYSICS_OBSTACLE_COMPONENT_H
#define _PHYSICS_OBSTACLE_COMPONENT_H

#include "vphys/vps.h"

struct PhysicsObstacleComponent final
{
public:
    PhysicsObstacleComponent(vp::Obstacle* obstacle);
    vp::Obstacle* m_obstacle;
};

#endif