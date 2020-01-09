#ifndef _PHYSICS_AGENT_COMPONENT_H
#define _PHYSICS_AGENT_COMPONENT_H

#include "vphys/vps.h"

struct PhysicsAgentComponent final
{
    PhysicsAgentComponent(vp::VerletAgent* agent);
    vp::VerletAgent* m_agent;
};

#endif