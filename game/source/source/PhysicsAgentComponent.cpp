#include "PhysicsAgentComponent.h"


using namespace mt;
using namespace vp;

PhysicsAgentComponent::PhysicsAgentComponent( VerletAgent* agent )
    : m_agent(agent)
    , m_hitImpact(0.0f)
{    
    EASSERT(agent != nullptr);
}
