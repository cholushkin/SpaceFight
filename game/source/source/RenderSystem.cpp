#include "RenderSystem.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "PlayerComponent.h"
#include "EnergyResourceComponent.h"
#include "ExplosionEffectComponent.h"
#include "EnergyStationComponent.h"
#include "PlanetComponent.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
#include "screengameplay.h"
#include "config.h"
#include "res/sheet_gameplay.h"
#include "res/ani_expl.h"
#include "ext/math/mt_random.h"
#include "Level.h"

using namespace mt;
using namespace r;

RenderSystem::RenderSystem()
    : m_planetSprites{ gameplay::Planet1, gameplay::Planet2, gameplay::Planet3,gameplay::Planet4, gameplay::Planet5, gameplay::Planet6 }
    , m_bgSprites{ gameplay::SpaceBackgroundChunk1, gameplay::SpaceBackgroundChunk2, gameplay::SpaceBackgroundChunk3, gameplay::SpaceBackgroundChunk4 }
    , m_bgPositions{ {0.0f, 100.0f}, {100.0f, 200.0f}, {200.0f, 300.0f}, {300.0f, 400.0f} }
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
    {
        m_planetRotations[i] = RandomHelper::s_rnd.frand() * (float)M_PI;
        m_planetRotationSpeed[i] = (RandomHelper::s_rnd.frand() - 0.5f) * 0.5f;
    }

    for (int i = 0; i < ARRAY_SIZE(m_bgAngles); ++i)
    {
        m_bgAngles[i] = RandomHelper::s_rnd.frand() * (float)M_PI;
        m_bgAlphas[i] = RandomHelper::s_rnd.frand() * 0.5f;
    }
}

void RenderSystem::Update(float dt, entt::DefaultRegistry& registry)
{
    for (int i = 0; i < ARRAY_SIZE(m_planetRotations); ++i)
        m_planetRotations[i] += m_planetRotationSpeed[i] * dt;

    for (int i = 0; i < ARRAY_SIZE(m_bgAlphas); ++i)
        m_bgAlphas[i] += dt * 0.02f;

    // update effects
    registry.view<ExplosionEffectComponent>().each(
        [&](auto ent, ExplosionEffectComponent& explComp)
    {
        explComp.UpdateProgress(dt);
        if (explComp.m_progress == 1.0f) // todo: need to have decent animation sprite class...
            registry.destroy(ent);
    });
}

void RenderSystem::Render(r::Render& r, GameResources& gRes, entt::DefaultRegistry& registry)
{
    DrawHelper dr(r);
    static const v2f offset = v2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
    dr.SetTransform(offset);

    // background
    int bgIndex = 0;
    registry.view<v2f>().each([&](auto /*entity*/, v2f& pos)
    {
        gRes.m_sheet->Draw(r, m_bgSprites[bgIndex], pos + offset,
            0x44ffffff, 8.0f, m_bgAlphas[bgIndex], true);
        ++bgIndex;
    });

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
                m_planetSprites[planetComp.m_planetType],
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

            if (stationComp.HasEnergy())
                registry.view<PlayerComponent, PhysicsAgentComponent>().each([&](auto /*playerEnt*/, PlayerComponent& /*playerComp*/, PhysicsAgentComponent& playerPsxComp)
            {
                auto distance = (playerPsxComp.m_agent->pos - psxComp.m_agent->pos).length();
                if (distance < SHIP_IMPACT_RADIUS)
                {
                    dr.DrawLine(playerPsxComp.m_agent->pos, psxComp.m_agent->pos, COLOR_ORANGE);
                }
            });
        }

        if (entTypeComp.m_entityType == EntityTypeComponent::Ship)
        {
            const auto& playerComp = registry.get<PlayerComponent>(entity);
            gRes.m_sheet->Draw(
                r,
                playerComp.m_playerID == 0 ? gameplay::Ship1 : gameplay::Ship2,
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 1.0f,
                atan2(psxComp.m_agent->deltaPos.y, psxComp.m_agent->deltaPos.x) + (float)M_PI_2,
                true
            );

            gRes.m_sheet->Draw(
                r,
                gameplay::ShipImpactCircle,
                psxComp.m_agent->pos + offset,
                COLOR_WHITE, 1.25f);

            if (playerComp.m_energy > 0.0f)
                gRes.m_sheet->Draw(
                    r,
                    gameplay::ShipShield,
                    psxComp.m_agent->pos + offset,
                    0x55ffffff, 1.6f);
        }
    });

    // draw impact radius
    registry.view<PhysicsAgentComponent, PlayerComponent>().each([&]
    (auto /*entity*/, PhysicsAgentComponent& psxComp, PlayerComponent& playerComp)
    {
        bool isInPower = playerComp.m_energy > 0.0f;
        if (isInPower)
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

    // draw effects
    registry.view<ExplosionEffectComponent>().each(
        [&](auto /*ent*/, ExplosionEffectComponent& explComp)
    {
        gRes.m_explSprite->Draw(r,
            (u32)(explComp.m_progress * (ani_expl::GetNFrames(ani_expl::aExp) - 1)),
            explComp.m_pos + offset); // todo: need to have decent animation sprite class...
    });

#ifdef _DEBUG
    dr.DrawStar(v2f(), 10.0f, 4, COLOR_YELLOW); // zero axis

    // draw physics body wire
    registry.view<PhysicsAgentComponent>().each([&](auto /*entity*/, PhysicsAgentComponent& psxComp)
    {
        dr.DrawCircle(psxComp.m_agent->pos, psxComp.m_agent->radius, 16, COLOR_WHITE);
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

