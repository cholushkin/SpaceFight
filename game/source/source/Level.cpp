#include "Level.h"
#include "config.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"
#include "EntityTypeComponent.h"
#include "EnergyResourceComponent.h"
#include "EnergyStationComponent.h"
#include "PhysicsAgentComponent.h"
#include "res/sheet_gameplay.h"
#include "screengameplay.h"
#include <time.h> 

using namespace mt;

// GenerationOptions
// ---------------------------------------------------------------
Level::GenerationOptions::GenerationOptions()
    : PlanetsAmmount(2, 6)
    , EnergyStationAmmount(1, 3)
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
    auto planetMaxAmount = RandomHelper::RndRange(m_options.PlanetsAmmount);
    for (int i = 0; i < planetMaxAmount; ++i)
    {
        v2f rndPos((float)RandomHelper::RndRange(100, SCREEN_WIDTH - 100), (float)RandomHelper::RndRange(100, SCREEN_HEIGHT - 100));
        rndPos -= v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreatePlanet(rndPos);
        }
    }

    // create energy stations
    auto stationsMaxAmount = RandomHelper::RndRange(m_options.EnergyStationAmmount);
    float radius = static_cast<float>(RandomHelper::RndRange(250, 350));
    for (int i = 0; i < stationsMaxAmount; ++i)
    {
        v2f rndPos = RandomHelper::RndPointOnCircle(v2f(), radius);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreateEnergyStation(rndPos);
        }
    }

    // create energy pickups
    auto energyPickupsMaxAmount = RandomHelper::RndRange(m_options.EnergyPickupsAmmount);
    for (int i = 0; i < energyPickupsMaxAmount; ++i)
    {
        v2f rndPos((float)RandomHelper::RndRange(10, SCREEN_WIDTH - 10), (float)RandomHelper::RndRange(10, SCREEN_HEIGHT - 10));
        rndPos -= v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreateEnergyPickup(rndPos, v2f(RandomHelper::s_rnd.frand(), RandomHelper::s_rnd.frand()));
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


void Level::CreatePlasmaBullet(const mt::v2f& pos, const mt::v2f& direction)
{
    const auto plasmaBullet = m_registry.create();
    m_registry.assign<EntityTypeComponent>(plasmaBullet, EntityTypeComponent::PlasmaBullet);
    m_registry.assign<EnergyResourceComponent>(plasmaBullet);
    auto& psx = m_registry.assign<PhysicsAgentComponent>(
        plasmaBullet,
        m_physicsSystem.AddAgent(plasmaBullet, pos, 0.0f, 6.0f, false)
        );
    psx.m_agent->Push(direction * 15.0f);
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
    m_registry.assign<PlanetComponent>(planet, RandomHelper::s_rnd.rand() % 6, 1.0f, 100.0f, 50.0f);
    auto psxAgentComp = m_registry.assign<PhysicsAgentComponent>(
        planet,
        m_physicsSystem.AddAgent(planet, pos, 0.0f, 50.0f, true)
        );
}

void Level::CreateEnergyStation(const mt::v2f& pos)
{
    const auto station = m_registry.create();
    m_registry.assign<EntityTypeComponent>(station, EntityTypeComponent::EnergyStation);
    m_registry.assign<EnergyStationComponent>(station, ENERGY_STATION_CAPACITY);
    auto psxAgentComp = m_registry.assign<PhysicsAgentComponent>(
        station,
        m_physicsSystem.AddAgent(station, pos, 0.0f, 10.0f, true)
        );
}


bool Level::IsHit(const v2f& pos)
{
    for (const v2f& c : m_created)
        if ((c - pos).length() < 100.0f)
            return true;
    return false;
};


// RandomHelper
// --------------------------------------
mt::fast_random_generator RandomHelper::s_rnd((u32)time(nullptr));
