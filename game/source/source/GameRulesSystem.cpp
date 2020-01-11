#include "GameRulesSystem.h"
#include "EnergyResourceComponent.h"
#include "EnergyStationComponent.h"
#include "PlayerComponent.h"
#include "config.h"
#include "core/math/mt_base.h"
#include <algorithm>
#include <iterator>

using namespace mt;

GameRulesSystem::GameRulesSystem(Level& level )
    : m_winnerID(-1)
    , m_level(level)
{
}

bool GameRulesSystem::HasWinner() const
{
    return m_winnerID >= 0;
}

bool GameRulesSystem::GetWinnerID() const
{
    return m_winnerID;
}
 
void GameRulesSystem::Update(float dt, entt::DefaultRegistry& registry)
{
    auto playersView = registry.view<PlayerComponent>();

    // check win conditions
    if (!HasWinner())
    {
        if (playersView.size() < 2)
            playersView.each([&](auto /*ent*/, PlayerComponent& playerComp)
        {
            playerComp.m_winCount++;
            m_winnerID = playerComp.m_playerID;
        });
    }    

    // energy stations
    registry.view<EnergyStationComponent>().each([&](auto /*stationEnt*/, EnergyStationComponent& stationComp)
    {        
        stationComp.m_energy += ENERGY_STATION_RECOVER_SPEED * dt; 
        stationComp.m_energy = Clamp(stationComp.m_energy, 0.0f, ENERGY_STATION_CAPACITY);
    });

    // retriever progress
    registry.view<EnergyResourceComponent>().each(
        [&](auto energyResEnt, EnergyResourceComponent& enrgRes)
    {
        for (int i = 0; i < MAX_PLAYERS; ++i)
            enrgRes.m_retriveProgression[i] = mt::Clamp(enrgRes.m_retriveProgression[i] - dt * 0.25f, 0.0f, 3.0f); // recover 
        
        playersView.each([&](auto /*playerEnt*/, PlayerComponent& playerComp) // collect
        {
            if ( enrgRes.m_retriveProgression[playerComp.m_playerID] > RETRIEVE_IMPACT_MAX)
            {
                playerComp.EnergyRecieve(ENERGY_REWARD_PLASMA_COLLECTED);
                m_level.DeleteEntity(energyResEnt);
            }
        });
        
    });
}
