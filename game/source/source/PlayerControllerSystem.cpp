#include "screengameplay.h"
#include "PlayerControllerSystem.h"
#include "PhysicsAgentComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "EnergyStationComponent.h"
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
        auto isInPower = playerComp.m_energy > 0.0f;

        if (isInPower)
        {
            // update engines
            auto input = m_inputSystem.GetVectorFromInput(playerComp.m_playerID);
            if (playerComp.EnergySpend(input.length()* 0.1f))
                psxComp.m_agent->ApplyAcceleration(ENGINE_ACCELERATION * input);
            else
                playerComp.m_energy = 0.f;


            // update retriever
            {
                registry.view<EnergyResourceComponent>().each(
                    [&](auto ent, EnergyResourceComponent& energyRes)
                {                    
                    auto distance = (psxComp.m_agent->pos - registry.get<PhysicsAgentComponent>(ent).m_agent->pos).length();
                    if (distance < SHIP_IMPACT_RADIUS)
                        energyRes.m_retriveProgression[playerComp.m_playerID] += dt;
                });

                registry.view<EnergyStationComponent>().each(
                    [&](auto stationEnt, EnergyStationComponent& stationComp)
                {
                    auto distance = (psxComp.m_agent->pos - registry.get<PhysicsAgentComponent>(stationEnt).m_agent->pos).length();
                    if (distance < SHIP_IMPACT_RADIUS)
                    {
                        auto energyDelta = ENERGY_STATION_CHARGE_SPEED * dt;
                        if (stationComp.m_energy >= energyDelta)
                        {
                            stationComp.m_energy -= energyDelta;
                            playerComp.EnergyRecieve(energyDelta);
                        }                        
                    }
                });
            }

            // update guns
            playerComp.m_weaponStatus += dt;
            if (m_inputSystem.GetShootButton(playerComp.m_playerID) && playerComp.m_weaponStatus >= 0.5f) // shoot plasma
            {
                v2f enemyDirection = GetDistanceToClosestEnemy(entity, registry); // get direction to enemy            
                if (enemyDirection.length() != 0.0f && playerComp.EnergySpend(SHIP_SHOOT_COST))
                {
                    playerComp.m_weaponStatus = 0.0f;
                    m_level.CreatePlasmaBullet(psxComp.m_agent->pos, enemyDirection);
                }
            }
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