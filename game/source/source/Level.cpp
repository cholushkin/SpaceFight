#include "Level.h"


#include "ext/math/mt_random.h"
#include "config.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"
#include "PhysicsAgentComponent.h"
#include "res/sheet_gameplay.h"
#include <time.h> 

using namespace mt;

// GenerationOptions
// ---------------------------------------------------------------
mt::fast_random_generator g_rnd((u32)time(nullptr));
inline int rndRange(int min, int max) // [min, max]
{
    return min + g_rnd.rand() % ((max + 1) - min);
}
//
//v2f rndInRect(const v2f& center, float width, float height)
//{
//
//}

Level::GenerationOptions::GenerationOptions()
    : PlanetsAmmount(2, 6)
    , EnergyStationAmmount(1, 5)
    , AsteroidsAmmount(10, 30)
{
}



// Level
// ---------------------------------------------------------------


Level::Level(entt::DefaultRegistry& registry, PhysicsSystem& psx)
    : m_registry(registry)
    , m_physicsSystem(psx)
{
}

void Level::CreateLevelRandom()
{
    // clear occupation array
    m_created.clear();

    // create 2 players for now (hardcoded)
    CreatePlayerEntity(0, v2f(-200.0f, 0.0f));
    CreatePlayerEntity(1, v2f(200.0f, 0.0f));

    static auto IsHit = [&](const v2f& pos) {
        for (auto&& c : m_created)
            if ((c - pos).length() < 100)
                return true;
        return false;
    };

    auto planetMaxAmount = rndRange(m_options.PlanetsAmmount.x, m_options.PlanetsAmmount.y);
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
}

void Level::CreatePlayerEntity(int playerID, const mt::v2f& pos)
{
    const auto player = m_registry.create();
    auto playerComponent = m_registry.assign<PlayerComponent>(player, playerID);
    auto physicsAgentComponent = m_registry.assign<PhysicsAgentComponent>(
        player, m_physicsSystem.AddAgent(player, pos, false));
    m_created.push_back(pos);
}

void Level::DeleteEntity(uint32_t& entt)
{
    if (m_registry.has<PhysicsAgentComponent>(entt))
        m_physicsSystem.RemoveAgent(entt);
    m_registry.destroy(entt);
}

void Level::CreatePlanet(const v2f& pos)
{
    static const int planetType[] = { gameplay::Planet1, gameplay::Planet2, gameplay::Planet3,
        gameplay::Planet4, gameplay::Planet5, gameplay::Planet6 };

    const auto planet = m_registry.create();
    m_registry.assign<PlanetComponent>(planet, (int)planetType[g_rnd.rand() % ARRAY_SIZE(planetType)], 1.0f, 100.0f, 50.0f);
    auto psxAgentComp = m_registry.assign<PhysicsAgentComponent>(
        planet,
        m_physicsSystem.AddAgent(planet, pos, true)
        );
    psxAgentComp.m_agent->radius = 50.0f;
}
