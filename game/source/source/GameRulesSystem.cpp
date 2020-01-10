#include "GameRulesSystem.h"
#include "EnergyResourceComponent.h"
#include "PlayerComponent.h"
#include "config.h"
#include "core/math/mt_base.h"

using namespace mt;

GameRulesSystem::GameRulesSystem()
    : m_winnerID(-1)
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
    // retriever progression recover
    registry.view<EnergyResourceComponent>().each(
        [&](auto /*ent*/, EnergyResourceComponent& eRus)
    {
        for (int i = 0; i < MAX_PLAYERS; ++i)
            eRus.m_retriveProgression[i] = mt::Clamp(eRus.m_retriveProgression[i] - dt * 0.25f, 0.0f, 3.0f);
    });

    // check win conditions
    if (!HasWinner())
    {
        auto view = registry.view<PlayerComponent>();
        if (view.size() < 2)
            view.each([&](auto /*ent*/, PlayerComponent& playerComp)
        {
            playerComp.m_winCount++;
            m_winnerID = playerComp.m_playerID;
        });
    }
}
