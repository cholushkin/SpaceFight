#include "RenderSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "PlanetComponent.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
#include "screengameplay.h"
#include "config.h"
#include "res/sheet_gameplay.h"
#include "ext/math/mt_random.h"

using namespace mt;
using namespace r;


u32 planetSprites[] = { gameplay::Planet1, gameplay::Planet2, gameplay::Planet3,
gameplay::Planet4, gameplay::Planet5, gameplay::Planet6 };
float m_planetRotations[6];
float m_planetRotationSpeed[6];
float m_bgRotations[4];

extern mt::fast_random_generator g_rnd;

RenderSystem::RenderSystem()
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
    {
        m_planetRotations[i] = g_rnd.frand() * (float)M_PI;
        m_planetRotationSpeed[i] = (g_rnd.frand() - 0.5f) * 0.5f;
    }

    for (int i = 0; i < ARRAY_SIZE(m_bgRotations); ++i)
        m_bgRotations[i] = g_rnd.frand() * (float)M_PI;
}

void RenderSystem::Update(float dt, entt::DefaultRegistry& /*registry*/)
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
        m_planetRotations[i] += m_planetRotationSpeed[i] * dt;
}

void RenderSystem::Render(r::Render& r, GameResources& gRes, entt::DefaultRegistry& registry)
{
    DrawHelper dr(r);
    static const v2f offset = v2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

    // background
    gRes.m_sheet->Draw(r, gameplay::SpaceBackgroundChunk1, v2f() + offset, COLOR_WHITE, 8.0f);
    gRes.m_sheet->Draw(r, gameplay::SpaceBackgroundChunk3, v2f(333,333) + offset, COLOR_WHITE, 8.0f);

    // planets
    registry.view<PhysicsAgentComponent, PlanetComponent>().each(
        [&](auto /*entity*/, PhysicsAgentComponent& psxComp, PlanetComponent& planetComp)
    {
        gRes.m_sheet->Draw(
            r,
            planetSprites[planetComp.m_planetType],
            psxComp.m_agent->pos + offset,
            COLOR_WHITE, 1.0f,
            m_planetRotations[planetComp.m_planetType], true);
    });

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
            psx.m_agent->pos + v2f(erComp.m_retriveProgression[0] * 10.0f, 0.0f), COLOR_AQUA);

        dr.DrawLine(
            psx.m_agent->pos,
            psx.m_agent->pos + v2f(erComp.m_retriveProgression[1] * 10.0f, 0.0f) + v2f(0.0, 1.0f), COLOR_AQUA);
    });
#endif
}

