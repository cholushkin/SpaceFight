#include "screengameplay.h"
#include "PlayerControllerSystem.h"
#include "PhysicsAgentComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "config.h"

using namespace mt;

PlayerControllerSystem::PlayerControllerSystem(InputSystem& input, Level& level)
    : m_inputSystem(input)
    , m_level(level)
{
}

void PlayerControllerSystem::Update(float dt, entt::DefaultRegistry& registry)
{
    registry.view<PlayerComponent, PhysicsAgentComponent>().each(
        [&](auto entity,
            PlayerComponent& playerComp,
            PhysicsAgentComponent& psxComp)
    {
        // update engines
        auto input = m_inputSystem.GetVectorFromInput(playerComp.m_playerID);
        psxComp.m_agent->ApplyAcceleration(ENGINE_ACCELERATION * input);
        playerComp.m_energy -= input.length()* 0.1f;

        // update retriever
        {
            registry.view<EnergyResourceComponent>().each(
                [&](auto ent, EnergyResourceComponent& eRus)
            {
                //auto& tst = registry.get<PhysicsAgentComponent>(ent);
                //EASSERT(tst.m_agent != nullptr);
                auto distance = (psxComp.m_agent->pos - registry.get<PhysicsAgentComponent>(ent).m_agent->pos).length();
                if (distance < SHIP_IMPACT_RADIUS)
                {
                    eRus.m_retriveProgression[playerComp.m_playerID] += dt;
                    if (eRus.m_retriveProgression[playerComp.m_playerID] > RETRIEVE_IMPACT_MAX)
                    {
                        m_level.DeleteEntity(ent);
                        playerComp.m_energy += ENERGY_REWARD_PLASMA_COLLECTED;
                    }
                }
            });
        }

        // update guns
        playerComp.m_weaponStatus += dt;
        if (m_inputSystem.GetShootButton(playerComp.m_playerID) && playerComp.m_weaponStatus >= 0.5f) // shoot plasma
        {
            playerComp.m_weaponStatus = 0.0f;            
            v2f enemyDirection = GetDistanceToClosestEnemy(entity, registry); // get direction to enemy            
            m_level.CreatePlasmaBullet(psxComp.m_agent->pos, enemyDirection);
            playerComp.m_energy -= SHIP_SHOOT_COST;
        }
    });
}

v2f PlayerControllerSystem::GetDistanceToClosestEnemy(const uint32_t& from, entt::DefaultRegistry& registry)
{
    // note: for now just opposite player
    v2f distance;
    registry.view<PlayerComponent, PhysicsAgentComponent>().each(
        [&](auto entt, PlayerComponent& /*playerComp*/, PhysicsAgentComponent& psx)
    {
        if (entt != from)
            distance = psx.m_agent->pos - registry.get<PhysicsAgentComponent>(from).m_agent->pos;
    });
    return distance;
}