#include "screengameplay.h"

#include "config.h"
#include "playertag.h"
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
  //  m_fnt = GetFont(RES_COOPER32_L1R_BMFC_FNT, m_resPool, g.GetRender());
  //  m_fntBase24 = GetFont(RES_COOPER24_L1R_BMFC_FNT, m_resPool, g.GetRender());

  //  m_gameTiles[sBacto] = bactos::GetSprite(g.GetRender(), m_resPool);
  //  m_gameTiles[sBlock] = blocks::GetSprite(g.GetRender(), m_resPool);
  //  m_gameTiles[sTiles] = tiles::GetSprite(g.GetRender(), m_resPool);
  //  m_gameTiles[sSnake] = snake::GetSprite(g.GetRender(), m_resPool);

  //  m_bactos = m_gameTiles[sBacto]; // somekind alias

  ////  EASSERT(m_gameTiles[GameResources::sBacto]->GetNFrames() == m_gameTiles[GameResources::sEyes]->GetNFrames());

  //  m_popupGo = popup_go::GetSprite(g.GetRender(), m_resPool);
  //  m_hud = hud::GetSprite(g.GetRender(), m_resPool);
  //  m_entIcos = entity_icon::GetSprite(g.GetRender(), m_resPool);
  //  m_grayvin.m_pTexture = g.GetRender().GetTexture(RES_GRAYVIN_PNG, m_resPool);
  //  m_soupSheet = soup::GetSprite(g.GetRender(), m_resPool);
  //  m_gameTileset.SetSheet(m_gameTiles[sTiles]);
  //  m_customMsg = new GoBravoMessage(m_popupGo, GoBravoMessage::GO);

  //  // sounds

  //  // gameplay
  //  m_boom = g.GetSMGR().GetSound(RES_BOOM_MP3, m_resPool);
  //  m_eat_food = g.GetSMGR().GetSound(RES_EAT_FOOD_MP3, m_resPool);
  //  m_eater_feed_end = g.GetSMGR().GetSound(RES_EATER_FEED_END_MP3, m_resPool);
  //  m_eater_feed_start = g.GetSMGR().GetSound(RES_EATER_FEED_START_MP3, m_resPool);
  //  m_eater_feed_step = g.GetSMGR().GetSound(RES_EATER_FEED_STEP_MP3, m_resPool);
  //  m_grow_step = g.GetSMGR().GetSound(RES_GROW_STEP_MP3, m_resPool);
  //  m_move_eater_start = g.GetSMGR().GetSound(RES_MOVE_EATER_START_MP3, m_resPool);
  //  m_move_eater_step = g.GetSMGR().GetSound(RES_MOVE_EATER_STEP_MP3, m_resPool);
  //  m_move_stuff = g.GetSMGR().GetSound(RES_MOVE_STUFF_MP3, m_resPool);
  //  m_snake_eat = g.GetSMGR().GetSound(RES_SNAKE_EAT_MP3, m_resPool);
  //  m_button_click = g.GetSMGR().GetSound(RES_BUTTON_CLICK_MP3, m_resPool);

  //  m_bacto_connect = g.GetSMGR().GetSound(RES_BACTO_CONNECT_MP3, m_resPool);
  //  m_block_move = g.GetSMGR().GetSound(RES_BLOCK_MOVE_MP3, m_resPool);
  //  m_snake_move = g.GetSMGR().GetSound(RES_SNAKE_MOVE_MP3, m_resPool);
  //  m_eater_die = g.GetSMGR().GetSound(RES_EATER_DIE_MP3, m_resPool);
  //  m_move_slimmed = g.GetSMGR().GetSound(RES_MOVE_SLIMMED_MP3, m_resPool);
  //  m_tutor_start = g.GetSMGR().GetSound(RES_TUTOR_START_MP3, m_resPool);


  //  // messages
  //  m_snd_go = g.GetSMGR().GetSound(RES_GO_MP3, m_resPool);
  //  m_snd_bravo = g.GetSMGR().GetSound(RES_BRAVO_MP3, m_resPool);

  //  // UI
  //  m_panel_show = g.GetSMGR().GetSound(RES_PANEL_SHOW_MP3, m_resPool);
  //  m_panel_hide = g.GetSMGR().GetSound(RES_PANEL_HIDE_MP3, m_resPool);
  //  LoadPS
}


// ScreenGameplay 
// ---------------------------------------------------------------
ScreenGameplay::ScreenGameplay(Application& app)
    : m_app(app)
    , m_time(0.0f)
    , m_res(app)
    , m_gameState(Playing)
{
    const auto player = m_registry.create();
    m_registry.attach<PlayerTag>(player);
    /*m_registry.assign<SpriteComponent>(player, 12, 96, SDL_Colour{ 255, 255, 255, 255 });
    m_registry.assign<PositionComponent>(player, 20.0, 20.0);*/
}

ScreenGameplay::~ScreenGameplay()
{
}

void ScreenGameplay::Update(f32 dt) 
{
    m_time += dt;

    m_inputSystem.Update(dt, m_registry);

}

void ScreenGameplay::Draw(r::Render& /*r*/)
{
}
