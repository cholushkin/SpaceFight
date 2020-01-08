#include "PhysicsAgentComponent.h"


using namespace mt;
using namespace vp;

PhysicsAgentComponent::PhysicsAgentComponent( vp::VerletAgent* agent )
    : m_agent(agent)
{    
    EASSERT(agent != nullptr);
}
