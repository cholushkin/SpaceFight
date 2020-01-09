#include "PhysicsSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"
#include "EnergyResourceComponent.h"
#include "config.h"


using namespace vp;

PhysicsSystem::PhysicsSystem(entt::DefaultRegistry& registry)
    : m_vps(AABB(-SCREEN_WIDTH/2.0f, SCREEN_WIDTH / 2.0f, -SCREEN_HEIGHT/ 2.0f, SCREEN_HEIGHT / 2.0f))
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

vp::VerletAgent* PhysicsSystem::AddAgent(const uint32_t& ent, const mt::v2f pos, float friction, float radius, bool isStatic)
{
    auto vAgent = new VerletAgent(VerletAgent::Descriptor(pos, radius, friction));
    vAgent->m_entity = ent;
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

static const float GravConst = 1000.25f;
void PhysicsSystem::Update(float dt, entt::DefaultRegistry& /*registry*/)
{
    m_vps.ProcessTime(dt);

    // apply gravity only for the players
    m_registry.view<PlayerComponent, PhysicsAgentComponent>().each([&](auto /*playerEnt*/, PlayerComponent& /*playerComp*/, PhysicsAgentComponent& playerPsxComp)
    {
        m_registry.view<PlanetComponent, PhysicsAgentComponent>().each([&](auto /*planetEnt*/, PlanetComponent& planetComp, PhysicsAgentComponent& planetPsxComp)
        {
            auto dDist = playerPsxComp.m_agent->pos - planetPsxComp.m_agent->pos;
            auto force = GravConst * planetComp.m_mass * planetComp.m_scale / dDist.lengthSq();
            playerPsxComp.m_agent->ApplyAcceleration(-dDist * force);
        });
    });

    // retriever progression recover
    m_registry.view<EnergyResourceComponent>().each(
        [&](auto /*ent*/, EnergyResourceComponent& eRus)
    {
        for (int i = 0; i < MAX_PLAYERS; ++i)
            eRus.m_retriveProgression[i] = mt::Clamp(eRus.m_retriveProgression[i] - dt * 0.25f, 0.0f, 3.0f);
    });
}

void PhysicsSystem::OnCollide(const Collision& collision)
{
    uint32_t bullet, ship;
    if (IsBulletToShipCollision(collision, bullet, ship))
    {
    }
}

bool PhysicsSystem::IsBulletToShipCollision(const Collision& /*collision*/, uint32_t& /*bullet*/, uint32_t& /*ship*/)
{
    return false;
    //EASSERT(collision.mP1 != nullptr);
    //EASSERT(collision.mP2 != nullptr);

    //m_registry.has<PhysicsAgentComponent>()

    //auto& psxComp = m_registry.get<PhysicsAgentComponent>(collision.mP1->m_entity);


    //m_registry.get<> collision.mP1->m_entity
}

