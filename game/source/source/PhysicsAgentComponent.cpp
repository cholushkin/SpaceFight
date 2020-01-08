#include "PhysicsAgentComponent.h"


using namespace mt;
using namespace vp;

PhysicsAgentComponent::PhysicsAgentComponent(vp::VerletAgent* agent )
    : m_agent(agent)
{    
}


//PhysicsAgentComponent::PhysicsAgentComponent(const PhysicsAgentComponent& pac)
//{
//    m_agent = pac.m_agent;
//}