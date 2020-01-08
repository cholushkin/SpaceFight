#include "PhysicsSystem.h"
#include "PhysicsAgentComponent.h"



using namespace vp;

PhysicsSystem::PhysicsSystem(entt::DefaultRegistry& registry)
    : m_vps(AABB())
    , m_registry(registry)
{
    m_vps.SetCollisionListener(this);
}

PhysicsSystem::~PhysicsSystem()
{
    m_registry.view<PhysicsAgentComponent>().each([&](auto /*entity*/, PhysicsAgentComponent& psx)
    {
        m_vps.RemoveParticle(psx.m_agent);
    });
}

vp::VerletAgent* PhysicsSystem::AddAgent(const uint32_t& /*ent*/, const mt::v2f pos)
{
    auto vAgent = new VerletAgent(VerletAgent::Descriptor(pos, 10.0f));
    m_vps.AddParticle(vAgent);
    return vAgent;
}

void PhysicsSystem::RemoveAgent(const uint32_t& entt)
{
    EASSERT(m_registry.has<PhysicsAgentComponent>(entt));
    auto comp = m_registry.get< PhysicsAgentComponent>(entt);
    EASSERT(comp.m_agent != nullptr);
    m_vps.RemoveParticle(comp.m_agent);
    delete comp.m_agent;
}


void PhysicsSystem::Update(float dt, entt::DefaultRegistry& /*registry*/)
{
    m_vps.ProcessTime(dt);
}

void PhysicsSystem::OnCollide(const Collision& /*collision*/)
{

}

