#include "screengameplay.h"

#include "config.h"
#include "playertag.h"
#include "PlayerComponent.h"

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


// ScreenGameplay 
// ---------------------------------------------------------------
ScreenGameplay::ScreenGameplay(Application& app)
    : m_app(app)
    , m_time(0.0f)
    , m_res(app)
    , m_gameState(Playing)
    , m_player1Dashboard(m_registry, m_res)
    , m_player2Dashboard(m_registry, m_res)
{
    // create 2 players for now (hardcoded)
    CreatePlayerEntity(0);
    CreatePlayerEntity(1);

    //m_registry.attach<PlayerTag>(player);
    /*m_registry.assign<SpriteComponent>(player, 12, 96, SDL_Colour{ 255, 255, 255, 255 });
    m_registry.assign<PositionComponent>(player, 20.0, 20.0);*/
}


ScreenGameplay::~ScreenGameplay()
{
}

void ScreenGameplay::Update(f32 dt) 
{
    m_time += dt;

    // update logic
    m_inputSystem.Update(dt, m_registry);

    // update gui
    m_player1Dashboard.Update(dt);
    m_player2Dashboard.Update(dt);

}

void ScreenGameplay::Draw(r::Render& r)
{
    m_player1Dashboard.Draw(r, v2f(16, 16));
    m_player2Dashboard.Draw(r, v2f(16, 64));
}

void ScreenGameplay::CreatePlayerEntity(int playerID)
{
    const auto player = m_registry.create();
    auto playerComponent = m_registry.assign<PlayerComponent>(player, playerID);
    if (playerID == 0)
        m_player1Dashboard.SetPlayer(player);
    if (playerID == 1)
        m_player2Dashboard.SetPlayer(player);
}
