#include "screengameplay.h"

#include "config.h"
#include "playertag.h"
#include "PlayerComponent.h"
#include "PlanetComponent.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "widgets.h"
#include "ext/math/mt_random.h"
#include <time.h> 
//#include "balance.h"
//#include "game.h"
//#include <algorithm>
//
//#include "core/io/io_base.h"
//#include "core/common/com_misc.h"
//#include "core/render/r_render.h"
//#include "str_helper.h"
//#include "ext/draw2d/draw_helper.h"
//#include "ext/math/mt_random.h"
//#include "screenmainmenu.h"
//#include "screenlevelselect.h"
//#include "music.h"
//
// --- res
#include "res/resources.h"
#include "res/sheet_gameplay.h"

//#include "res/sheet_bactos.h"
//#include "res/sheet_blocks.h"
//#include "res/sheet_hud.h"
//#include "res/sheet_snake.h"
//#include "res/sheet_popup_go.h"
//#include "res/sheet_soup.h"
//#include "res/sheet_entity_icon.h"
//#include "res/g_strings.h"

using namespace mt;
using namespace in;
using namespace r;


// GameResources 
// -----------------------------------------------------------------
GameResources::GameResources(Application& app)
    : m_resPool(64)
    , m_app(app)
{
    m_sheet = gameplay::GetSprite(app.GetRender(), app.GetRPool());
    m_fnt = GetFont(RES_8BIT_FNT, app.GetRPool(), app.GetRender());

    // sounds
    m_sfxExplode1 = m_app.GetSMGR().GetSound(RES_SFXEXPLODE1_WAV, m_resPool);
    m_sfxExplode2 = m_app.GetSMGR().GetSound(RES_SFXEXPLODE2_WAV, m_resPool);
    m_sfxExplode3 = m_app.GetSMGR().GetSound(RES_SFXEXPLODE3_WAV, m_resPool);
    m_sfxHit = m_app.GetSMGR().GetSound(RES_SFXHIT_WAV, m_resPool);
    m_sfxMenuApply = m_app.GetSMGR().GetSound(RES_SFXMENUAPPLY_WAV, m_resPool);
    m_sfxMenuSelect = m_app.GetSMGR().GetSound(RES_SFXMENUSELECT_WAV, m_resPool);
    m_sfxPlasmaHit = m_app.GetSMGR().GetSound(RES_SFXPLASMAHIT_WAV, m_resPool);
    m_sfxRefillEnergyStep = m_app.GetSMGR().GetSound(RES_SFXREFILLENERGYSTEP_WAV, m_resPool);
    m_sfxShoot = m_app.GetSMGR().GetSound(RES_SFXSHOOT_WAV, m_resPool);

    // particles systems
    LoadPS(m_particleSysExplosionA, RES_EXPLOSION1_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
    LoadPS(m_particleSysExplosionB, RES_EXPLOSION2_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
    LoadPS(m_particleSysTrail, RES_BURST_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
}


// LevelCreator
// ---------------------------------------------------------------
mt::fast_random_generator g_rnd((u32)time(nullptr));
inline int rndRange(int min, int max) // [min, max]
{
    return min + g_rnd.rand() % ((max + 1) - min);
}
//
//v2f rndInRect(const v2f& center, float width, float height)
//{
//
//}

LevelCreator::GenerationOptions::GenerationOptions()
    : PlanetsAmmount(2, 6)
    , EnergyStationAmmount(1, 5)
    , AsteroidsAmmount(10, 30)
{
}


LevelCreator::LevelCreator(entt::DefaultRegistry& registry, PhysicsSystem& psx)
    : m_registry(registry)
    , m_physicsSystem(psx)
{
}

void LevelCreator::CreateLevelRandom()
{
    // clear occupation array
    m_created.clear();

    // create 2 players for now (hardcoded)
    CreatePlayerEntity(0, v2f(-200.0f, 0.0f));
    CreatePlayerEntity(1, v2f(200.0f, 0.0f));

    static auto IsHit = [&](const v2f& pos) {
        for (auto&& c : m_created)
            if ((c - pos).length() < 100)
                return true;
        return false;
    };

    auto planetMaxAmount = rndRange(m_options.PlanetsAmmount.x, m_options.PlanetsAmmount.y);
    for (int i = 0; i < planetMaxAmount; ++i)
    {
        v2f rndPos((float)rndRange(100, SCREEN_WIDTH - 100), (float)rndRange(100, SCREEN_HEIGHT - 100));
        rndPos -= v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        if (!IsHit(rndPos))
        {
            m_created.push_back(rndPos);
            CreatePlanet(rndPos);
        }
    }
}

void LevelCreator::CreatePlayerEntity(int playerID, const mt::v2f& pos)
{
    const auto player = m_registry.create();
    auto playerComponent = m_registry.assign<PlayerComponent>(player, playerID);
    auto physicsAgentComponent = m_registry.assign<PhysicsAgentComponent>(
        player, m_physicsSystem.AddAgent(player, pos, false));
    m_created.push_back(pos);
}

void LevelCreator::DeleteEntity(uint32_t& entt)
{
    if (m_registry.has<PhysicsAgentComponent>(entt))
        m_physicsSystem.RemoveAgent(entt);
    m_registry.destroy(entt);
}

void LevelCreator::CreatePlanet(const v2f& pos)
{
    static const int planetType[] = { gameplay::Planet1, gameplay::Planet2, gameplay::Planet3,
        gameplay::Planet4, gameplay::Planet5, gameplay::Planet6 };
    
    const auto planet = m_registry.create();
    m_registry.assign<PlanetComponent>(planet, (int)planetType[g_rnd.rand() % ARRAY_SIZE(planetType)], 1.0f, 100.0f, 50.0f);
    auto psxAgentComp = m_registry.assign<PhysicsAgentComponent>(
        planet,
        m_physicsSystem.AddAgent(planet, pos, true)
        );
    psxAgentComp.m_agent->radius = 50.0f;

}


// ScreenGameplay 
// ---------------------------------------------------------------
ScreenGameplay::ScreenGameplay(Application& app)
    : m_app(app)
    , m_time(0.0f)
    , m_res(app)
    , m_levelCreator(m_registry, m_physicsSystem)
    , m_player1Dashboard(m_registry, m_res)
    , m_player2Dashboard(m_registry, m_res)
    , m_modalMessenger(m_registry, m_res)
    , m_playerControllerSystem(m_inputSystem)
    , m_physicsSystem(m_registry)

{
    SetState(Initialization);

    //m_registry.attach<PlayerTag>(player);
    /*m_registry.assign<SpriteComponent>(player, 12, 96, SDL_Colour{ 255, 255, 255, 255 });
    m_registry.assign<PositionComponent>(player, 20.0, 20.0);*/
}


ScreenGameplay::~ScreenGameplay()
{
}

void ScreenGameplay::Draw(r::Render& r)
{
    m_player1Dashboard.Draw(r, v2f(16, 16));
    m_player2Dashboard.Draw(r, v2f(16, 64 + 16));

    m_renderSystem.Render(r, m_registry);

    m_modalMessenger.Draw(r, v2f());
}

void ScreenGameplay::Update(f32 dt)
{
    m_time += dt;

    m_inputSystem.Update(dt, m_registry);

    // ----- update state
    if (m_gameState == Playing)
    {
        // update gameplay gui
        m_player1Dashboard.Update(dt);
        m_player2Dashboard.Update(dt);

        // update systems
        m_playerControllerSystem.Update(dt, m_registry);
        m_physicsSystem.Update(dt, m_registry);

    }
    else if (m_gameState == Message)
    {
    }
    else if (m_gameState == Paused)
    {
    }


    m_modalMessenger.Update(dt);

    // switch to the next state of the messenger
    if (m_modalMessenger.GetState() == WidgetModalMessage::VIEW_CLOSED)
    {
        if (m_gameState == Initialization)
            SetState(Playing);

    }
}

void ScreenGameplay::SetState(EnGameStates state)
{
    m_gameState = state;

    // ------ on enter state
    if (m_gameState == Initialization)
    {
        m_modalMessenger.SetState(WidgetModalMessage::VIEW_FIGHT);

        // create new level
        m_levelCreator.CreateLevelRandom();

        // assign widget to entity
        auto view = m_registry.view<PlayerComponent>();
        for (auto entity : view)
        {
            auto& pc = view.get(entity);
            if (pc.m_playerID == 0)
                m_player1Dashboard.SetPlayer(entity);
            if (pc.m_playerID == 1)
                m_player2Dashboard.SetPlayer(entity);
        }

        /* if (playerID == 0)
             m_player1Dashboard.SetPlayer(player);
         if (playerID == 1)
             m_player2Dashboard.SetPlayer(player);*/
    }
}

