#include "RenderSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "EnergyStationComponent.h"
#include "PlanetComponent.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
#include "screengameplay.h"
#include "config.h"
#include "res/sheet_gameplay.h"
#include "ext/math/mt_random.h"
#include "Level.h"

using namespace mt;
using namespace r;

u32 planetSprites[] = { gameplay::Planet1, gameplay::Planet2, gameplay::Planet3,
gameplay::Planet4, gameplay::Planet5, gameplay::Planet6 };
float m_planetRotations[6];
float m_planetRotationSpeed[6];

float m_bgAlphas[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float m_bgAngles[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
v2f m_bgPositions[4] = { {0.0f, 100.0f}, {100.0f, 200.0f}, {200.0f, 300.0f}, {300.0f, 400.0f} }; // initially it is treated as a range of radius but later as an abs pos


RenderSystem::RenderSystem()
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
    {
        m_planetRotations[i] = RandomHelper::s_rnd.frand() * (float)M_PI;
        m_planetRotationSpeed[i] = (RandomHelper::s_rnd.frand() - 0.5f) * 0.5f;
    }

    for (int i = 0; i < ARRAY_SIZE(m_bgAngles); ++i)
    {
        m_bgAngles[i] = RandomHelper::s_rnd.frand() * (float)M_PI;
        m_bgAlphas[i] = RandomHelper::s_rnd.frand();
        //auto r = RandomHelper::RndRange(m_bgPositions[i]);
        //m_bgPositions[i] = RandomHelper::
    }
}

void RenderSystem::Update(float dt, entt::DefaultRegistry& /*registry*/)
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
        m_planetRotations[i] += m_planetRotationSpeed[i] * dt;

    for (int i = 0; i < ARRAY_SIZE(m_bgAlphas); ++i)
        m_bgAlphas[i] += dt * 0.02f;
}

void RenderSystem::Render(r::Render& r, GameResources& gRes, entt::DefaultRegistry& registry)
{
    DrawHelper dr(r);
    static const v2f offset = v2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
    dr.SetTransform(offset);

    // background
    //gRes.m_sheet->Draw(r, gameplay::SpaceBackgroundChunk1, v2f() + offset, 0x44ffffff, 8.0f, m_bgAlphas[0],true);
    //gRes.m_sheet->Draw(r, gameplay::SpaceBackgroundChunk3, v2f(333,333) + offset, COLOR_WHITE, 8.0f);

    registry.view<PhysicsAgentComponent, EntityTypeComponent>().each(
        [&](auto entity, PhysicsAgentComponent& psxComp, EntityTypeComponent& entTypeComp)
    {
        // planets
        if (entTypeComp.m_entityType == EntityTypeComponent::Planet)
        {
            EASSERT(registry.has<PlanetComponent>(entity));
            const auto& planetComp = registry.get<PlanetComponent>(entity);

            gRes.m_sheet->Draw(
                r,
                planetSprites[planetComp.m_planetType],
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 1.0f,
                m_planetRotations[planetComp.m_planetType], true);
        }

        // plasma bullets
        if (entTypeComp.m_entityType == EntityTypeComponent::PlasmaBullet)
        {
            gRes.m_sheet->Draw(
                r,
                gameplay::PlasmaBullet,
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 2.0f);
        }

        // energy pickups
        if (entTypeComp.m_entityType == EntityTypeComponent::EnergyPickup)
        {
            gRes.m_sheet->Draw(
                r,
                gameplay::PickupEnergy,
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 2.0f);
        }

        // station
        if (entTypeComp.m_entityType == EntityTypeComponent::EnergyStation)
        {
            const auto& stationComp = registry.get<EnergyStationComponent>(entity);
            gRes.m_sheet->Draw(
                r,
                stationComp.HasEnergy() ? gameplay::StationFull : gameplay::StationEmpty,
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 1.0f);



            //dr.DrawLine(
            //    psxComp.m_agent->pos + v2f(-50.0f, 20.0f),
            //    psxComp.m_agent->pos + v2f(-50.0f, 20.0f) + v2f(stationComp.m_energy,0.0f), 
            //    COLOR_YELLOW);

            if(stationComp.HasEnergy())
                registry.view<PlayerComponent,PhysicsAgentComponent>().each([&](auto /*playerEnt*/, PlayerComponent& /*playerComp*/, PhysicsAgentComponent& playerPsxComp)
                {
                    auto distance = (playerPsxComp.m_agent->pos - psxComp.m_agent->pos).length();
                    if (distance < SHIP_IMPACT_RADIUS)
                    {
                        dr.DrawLine(playerPsxComp.m_agent->pos, psxComp.m_agent->pos, COLOR_ORANGE);
                    }
                });
        }
    });

#ifdef _DEBUG

    // draw physics body wire
    registry.view<PhysicsAgentComponent>().each([&](auto /*entity*/, PhysicsAgentComponent& psxComp)
    {        
        dr.DrawStar(v2f(), 10.0f, 4, COLOR_YELLOW);
        dr.DrawCircle(psxComp.m_agent->pos, psxComp.m_agent->radius, 16, COLOR_WHITE);
    });

    // draw impact radius
    registry.view<PhysicsAgentComponent, PlayerComponent>().each([&]
    (auto /*entity*/, PhysicsAgentComponent& psxComp, PlayerComponent& playerComp)
    {
        dr.DrawCircle(psxComp.m_agent->pos, SHIP_IMPACT_RADIUS, 16, COLOR_AQUA);
        
        bool isInPower = playerComp.m_energy > 0.0f;
        if(isInPower)
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

