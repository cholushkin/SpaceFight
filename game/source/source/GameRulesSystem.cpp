#include "GameRulesSystem.h"
#include "EnergyResourceComponent.h"
#include "config.h"
#include "core/math/mt_base.h"

using namespace mt;

GameRulesSystem::GameRulesSystem()
{
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
}
