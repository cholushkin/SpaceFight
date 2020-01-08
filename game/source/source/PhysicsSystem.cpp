#include "PhysicsSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"



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

vp::VerletAgent* PhysicsSystem::AddAgent(const uint32_t& /*ent*/, const mt::v2f pos, bool isStatic)
{
    auto vAgent = new VerletAgent(VerletAgent::Descriptor(pos, 10.0f, 0.1f));
    vAgent->m_isStatic = isStatic;
    m_vps.AddParticle(vAgent);
    return vAgent;
}

vp::Obstacle* PhysicsSystem::AddObstacle(const uint32_t& /*ent*/, const mt::v2f pos)
{
    auto vObstacle = new Obstacle(pos, 50.0f);
    m_vps.AddObstacle(vObstacle);
    return vObstacle;
}

void PhysicsSystem::RemoveAgent(const uint32_t& entt)
{
    EASSERT(m_registry.has<PhysicsAgentComponent>(entt));
    auto comp = m_registry.get<PhysicsAgentComponent>(entt);
    EASSERT(comp.m_agent != nullptr);
    m_vps.RemoveParticle(comp.m_agent);
    delete comp.m_agent;
}

void PhysicsSystem::RemoveObstacle(const uint32_t& entt)
{
    EASSERT(m_registry.has<PhysicsObstacleComponent>(entt));
    auto comp = m_registry.get<PhysicsObstacleComponent>(entt);
    EASSERT(comp.m_obstacle != nullptr);
    m_vps.RemoveObstacle(comp.m_obstacle);
    delete comp.m_obstacle;
}

static const float GravConst = 0.25f;
void PhysicsSystem::Update(float dt, entt::DefaultRegistry& /*registry*/)
{
    m_vps.ProcessTime(dt);

    // gravity only for players
    
    m_registry.view<PlayerComponent, PhysicsAgentComponent>().each([&](auto /*playerEnt*/, PlayerComponent& /*playerComp*/, PhysicsAgentComponent& playerPsxComp)
    {
        m_registry.view<PlanetComponent, PhysicsAgentComponent>().each([&](auto /*planetEnt*/, PlanetComponent& planetComp, PhysicsAgentComponent& planetPsxComp)
        {
            auto dDist = playerPsxComp.m_agent->pos - planetPsxComp.m_agent->pos;
            auto force = GravConst * planetComp.m_mass * planetComp.m_scale / dDist.lengthSq();
            playerPsxComp.m_agent->Push(-dDist * force);
        });
    });
}

void PhysicsSystem::OnCollide(const Collision& /*collision*/)
{

}

