#include "RenderSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "PlanetComponent.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
#include "config.h"

using namespace mt;
using namespace r;

void RenderSystem::Update(float /*dt*/, entt::DefaultRegistry& /*registry*/)
{

}

void RenderSystem::Render(r::Render& r, entt::DefaultRegistry& registry)
{
    DrawHelper dr(r);
    static const v2f offset = v2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

#ifdef _DEBUG

    // draw physics body wire
    registry.view<PhysicsAgentComponent>().each([&](auto /*entity*/, PhysicsAgentComponent& psxComp)
    {
        dr.SetTransform(offset);
        dr.DrawStar(v2f(), 10.0f, 4, COLOR_YELLOW);
        dr.DrawCircle(psxComp.m_agent->pos, psxComp.m_agent->radius, 16, COLOR_WHITE);
    });

    // draw impact radius
    registry.view<PhysicsAgentComponent, PlayerComponent>().each([&]
    (auto /*entity*/, PhysicsAgentComponent& psxComp, PlayerComponent& /*player*/)
    {
        dr.DrawCircle(psxComp.m_agent->pos, SHIP_IMPACT_RADIUS, 16, COLOR_AQUA);

        registry.view<EnergyResourceComponent>().each([&](auto ett, EnergyResourceComponent& /*erComp*/)
        {
            auto& psxEnergyRes = registry.get<PhysicsAgentComponent>(ett);
            auto distance = (psxComp.m_agent->pos - psxEnergyRes.m_agent->pos).length();
            if (distance < SHIP_IMPACT_RADIUS)
            {
                dr.DrawLine(psxComp.m_agent->pos, psxEnergyRes.m_agent->pos, COLOR_ORANGE);
            }
        });
    });

    // draw retrieve progression
    registry.view<EnergyResourceComponent>().each([&](auto ett, EnergyResourceComponent& erComp)
    {
        auto& psx = registry.get<PhysicsAgentComponent>(ett);
        dr.DrawLine(
            psx.m_agent->pos, 
            psx.m_agent->pos + v2f(erComp.m_retriveProgression[0]*10.0f,0.0f), COLOR_AQUA);

        dr.DrawLine(
            psx.m_agent->pos,
            psx.m_agent->pos + v2f(erComp.m_retriveProgression[1] * 10.0f, 0.0f) + v2f(0.0, 1.0f), COLOR_AQUA);
    });
#endif

    //registry.view<PhysicsAgentComponent, PlanetComponent>().each([&](PhysicsAgentComponent& psxCop, PlanetComponent& planetComp)
    //{
    //   
    //});
}

