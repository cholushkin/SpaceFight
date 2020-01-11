#include "Level.h"
#include "ext/math/mt_random.h"
#include "config.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"
#include "EntityTypeComponent.h"
#include "EnergyResourceComponent.h"
#include "PhysicsAgentComponent.h"
#include "res/sheet_gameplay.h"
#include "screengameplay.h"
#include <time.h> 

using namespace mt;

// GenerationOptions
// ---------------------------------------------------------------
mt::fast_random_generator g_rnd((u32)time(nullptr));
inline int rndRange(int min, int max) // [min, max]
{
    return min + g_rnd.rand() % ((max + 1) - min);
}
inline int rndRange(const Range& range)
{
    return rndRange(range.x, range.y);
}

Level::GenerationOptions::GenerationOptions()
    : PlanetsAmmount(2, 6)
    , EnergyStationAmmount(1, 5)
    , AsteroidsAmmount(10, 30)
    , EnergyPickupsAmmount(10,20)
{
}



// Level
// ---------------------------------------------------------------
Level::Level(entt::DefaultRegistry& registry, PhysicsSystem& psx, const SessionContext& sessionContext)
    : m_registry(registry)
    , m_physicsSystem(psx)
    , m_sessionContext(sessionContext)
{
}

void Level::CreateLevelRandom()
{
    // create 2 players for now (hardcoded)
    CreatePlayerEntity(0, v2f(-200.0f, 0.0f));
    CreatePlayerEntity(1, v2f(200.0f, 0.0f));

    // create planets
    auto planetMaxAmount = rndRange(m_options.PlanetsAmmount);
    for (int i = 0; i < planetMaxAmount; ++i)
    {
        v2f rndPos((float)rndRange(100, SCREEN_WIDTH - 100), (float)rndRange(100, SCREEN_HEIGHT - 100));
        rndPos -= v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreatePlanet(rndPos);
        }
    }

    // create energy pickups
    auto energyPickupsMaxAmount = rndRange(m_options.EnergyPickupsAmmount);
    for (int i = 0; i < energyPickupsMaxAmount; ++i)
    {
        v2f rndPos((float)rndRange(10, SCREEN_WIDTH - 10), (float)rndRange(10, SCREEN_HEIGHT - 10));
        rndPos -= v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreateEnergyPickup(rndPos, v2f(g_rnd.frand(), g_rnd.frand()));
        }
    }
}

void Level::CreatePlayerEntity(int playerID, const mt::v2f& pos)
{
    const auto player = m_registry.create();
    m_registry.assign<EntityTypeComponent>(player, EntityTypeComponent::Ship);
    m_registry.assign<PlayerComponent>(player, playerID, m_sessionContext.m_winCount[playerID]);
    m_registry.assign<PhysicsAgentComponent>(
        player, m_physicsSystem.AddAgent(player, pos, 0.1f, 10.0f, false));
    m_created.push_back(pos);
}

void Level::DeleteEntity(uint32_t& entt)
{
    if (m_registry.has<PhysicsAgentComponent>(entt))
        m_physicsSystem.RemoveAgent(entt);
    m_registry.destroy(entt);
}


void Level::CreatePlasmaBullet(const mt::v2f& pos, const mt::v2f& speed)
{
    const auto plasmaBullet = m_registry.create();
    m_registry.assign<EntityTypeComponent>(plasmaBullet, EntityTypeComponent::PlasmaBullet);
    m_registry.assign<EnergyResourceComponent>(plasmaBullet);
    auto& psx = m_registry.assign<PhysicsAgentComponent>(
        plasmaBullet,
        m_physicsSystem.AddAgent(plasmaBullet, pos, 0.0f, 6.0f, false)
        );
    psx.m_agent->Push(speed);
}

void Level::CreateEnergyPickup(const mt::v2f& pos, const mt::v2f& speed)
{
    const auto energyPickup = m_registry.create();
    m_registry.assign<EntityTypeComponent>(energyPickup, EntityTypeComponent::EnergyPickup);
    m_registry.assign<EnergyResourceComponent>(energyPickup);
    auto& psx = m_registry.assign<PhysicsAgentComponent>(
        energyPickup,
        m_physicsSystem.AddAgent(energyPickup, pos, 0.0f, 6.0f, false)
        );
    psx.m_agent->Push(speed);
}

void Level::CreatePlanet(const v2f& pos)
{
    const auto planet = m_registry.create();
    m_registry.assign<EntityTypeComponent>(planet, EntityTypeComponent::Planet);
    m_registry.assign<PlanetComponent>(planet, g_rnd.rand() % 6, 1.0f, 100.0f, 50.0f);
    auto psxAgentComp = m_registry.assign<PhysicsAgentComponent>(
        planet,
        m_physicsSystem.AddAgent(planet, pos, 0.0f, 50.0f, true)
        );
}


bool Level::IsHit(const v2f& pos)
{
    for (const v2f& c : m_created)
        if ((c - pos).length() < 100.0f)
            return true;
    return false;
};
