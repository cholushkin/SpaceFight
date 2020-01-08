#include "PlayerControllerSystem.h"
#include "PhysicsAgentComponent.h"
#include "PlayerComponent.h"

using namespace mt;

PlayerControllerSystem::PlayerControllerSystem(InputSystem& input)
    : m_inputSystem(input)
{
    
}

void PlayerControllerSystem::Update(float /*dt*/, entt::DefaultRegistry& registry)
{
    registry.view<PlayerComponent, PhysicsAgentComponent>().each(
        [&](auto /*entity*/,
            PlayerComponent& playerComp,
            PhysicsAgentComponent& psxComp)
    {
        //if (playerComp.m_playerID == 0)
        {
            auto input = m_inputSystem.GetVectorFromInput(playerComp.m_playerID);
            psxComp.m_agent->Push(input);
            //psxComp.m_agent->pos += input * 2.5f;
        }


        /*if (playerComp.m_playerID == 0)
        {
            
        }

        if (playerComp.m_playerID == 1)
        {
            psxComp.m_agent.Push(v2f(1.0f, 0.0f));
        }*/
    });
}

