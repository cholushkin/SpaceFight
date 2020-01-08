#ifndef _PHYSICS_AGENT_COMPONENT_H
#define _PHYSICS_AGENT_COMPONENT_H
//#include "ext/primitives/r_primitives.h"
#include "vphys/vps.h"

struct PhysicsAgentComponent final
{
public:
    PhysicsAgentComponent(vp::VerletAgent* agent);
    //PhysicsAgentComponent(const PhysicsAgentComponent& pac);

    //PhysicsAgentComponent() = default;
    //~PhysicsAgentComponent() = default;
    vp::VerletAgent* m_agent;
};

#endif