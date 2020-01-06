//#include "screengameplay.h"
//
//#include "config.h"
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
//// --- res
//#include "res/sheet_tiles.h"
//#include "res/sheet_bactos.h"
//#include "res/sheet_blocks.h"
//#include "res/sheet_hud.h"
//#include "res/sheet_snake.h"
//#include "res/sheet_popup_go.h"
//#include "res/sheet_soup.h"
//#include "res/sheet_entity_icon.h"
//#include "res/g_strings.h"
//
//// tuts
//#include "dlg_tutorial.h"
//
//using namespace mt;
//using namespace in;
//
////////////////////////////////////////////////////////////////////////////
//#define BRAVO_TIME 1.0f
//const int gBAD_ID = 999999;
//static fast_random_generator g_rnd;
//u32 ScreenGameplay::sLevIndx = 0;
//u32 ScreenGameplay::sLevPak = 0;
//Tune gParams;
//v2i8 gBadDragCell = mt::v2i8(125,125);
//#define FAILED_MSG_UID 33
//
//static const u32 g_fadeColor = ARGB(0xff,0x00,0x4a, 0x7F);
//
//extern void gCreateDecorVar1(Decor& decor,r::SheetSprite& sheet);
//
////  ----- Level index ---------------------------------------------
//
//inline void gIncLevelIndex(u32& levIndex) {
//  u32 curLevelPakMaxLevelIndex = gParams.LEVELS_COUNT_IN_LEVEL_PAK;
//  levIndex==curLevelPakMaxLevelIndex?levIndex:++levIndex;
//  bacto::gProcessMusicManager(bacto::MUS_GAMEPLAY);
//}
//
//// inline void gIncLevelIndex(u32& levIndex) {
////   levIndex = mt::Clamp<u32>(levIndex+1,0,gParams.LEVELS_COUNT_IN_LEVEL_PAK-1);
////   bacto::gProcessMusicManager(bacto::MUS_GAMEPLAY);
//// }
//
//inline void gDecLevelIndex(u32& levIndex) {
//  levIndex==0?0:--levIndex;
//  bacto::gProcessMusicManager(bacto::MUS_GAMEPLAY);
//}
//
//
//
//static bool isNeedTakeScreenshot = true;
//
//// ----- GameResources --------------------------------------------
//
//GameResources::GameResources( Game& g )
//:  m_resPool(64) 
//,  m_messengerResPool(4)
//,  m_customMsg()
//{
//  m_fntBase32 = GetFont(RES_COOPER32_L1R_BMFC_FNT,m_resPool,g.GetRender());
//  m_fntBase24 = GetFont(RES_COOPER24_L1R_BMFC_FNT,m_resPool,g.GetRender());
//  #ifdef DBG_DRAW_LEVEL_INDEX
//  
//  m_dbgText.SetText(m_fntBase24,L"");  
//  m_dbgText.SetFontScale(0.5f);
//#endif
//  m_gameTiles[sBacto] = bactos::GetSprite(g.GetRender(),m_resPool);
//  m_gameTiles[sBlock] = blocks::GetSprite(g.GetRender(),m_resPool);
//  m_gameTiles[sTiles] = tiles ::GetSprite(g.GetRender(),m_resPool);
//  m_gameTiles[sSnake] = snake ::GetSprite(g.GetRender(),m_resPool);
//
//  m_bactos = m_gameTiles[sBacto]; // somekind alias
//
////  EASSERT(m_gameTiles[GameResources::sBacto]->GetNFrames() == m_gameTiles[GameResources::sEyes]->GetNFrames());
//
//  m_popupGo     = popup_go::GetSprite(g.GetRender(),m_resPool);
//  m_hud         = hud::GetSprite(g.GetRender(),m_resPool);
//  m_entIcos     = entity_icon::GetSprite(g.GetRender(),m_resPool);
//  m_grayvin.m_pTexture = g.GetRender().GetTexture(RES_GRAYVIN_PNG, m_resPool);
//  m_soupSheet   = soup::GetSprite(g.GetRender(),m_resPool);
//  m_gameTileset.SetSheet(m_gameTiles[sTiles]);    
//  m_customMsg   = new GoBravoMessage(m_popupGo,GoBravoMessage::GO);
//
//  // sounds
//
//  // gameplay
//  m_boom             = g.GetSMGR().GetSound(RES_BOOM_MP3,             m_resPool);
//  m_eat_food         = g.GetSMGR().GetSound(RES_EAT_FOOD_MP3,         m_resPool);
//  m_eater_feed_end   = g.GetSMGR().GetSound(RES_EATER_FEED_END_MP3,   m_resPool);
//  m_eater_feed_start = g.GetSMGR().GetSound(RES_EATER_FEED_START_MP3, m_resPool);
//  m_eater_feed_step  = g.GetSMGR().GetSound(RES_EATER_FEED_STEP_MP3,  m_resPool);
//  m_grow_step        = g.GetSMGR().GetSound(RES_GROW_STEP_MP3,        m_resPool);
//  m_move_eater_start = g.GetSMGR().GetSound(RES_MOVE_EATER_START_MP3, m_resPool);
//  m_move_eater_step  = g.GetSMGR().GetSound(RES_MOVE_EATER_STEP_MP3,  m_resPool);
//  m_move_stuff       = g.GetSMGR().GetSound(RES_MOVE_STUFF_MP3,       m_resPool);
//  m_snake_eat        = g.GetSMGR().GetSound(RES_SNAKE_EAT_MP3,        m_resPool);
//  m_button_click     = g.GetSMGR().GetSound(RES_BUTTON_CLICK_MP3,     m_resPool);
//
//  m_bacto_connect    = g.GetSMGR().GetSound(RES_BACTO_CONNECT_MP3,    m_resPool);
//  m_block_move       = g.GetSMGR().GetSound(RES_BLOCK_MOVE_MP3,       m_resPool);
//  m_snake_move       = g.GetSMGR().GetSound(RES_SNAKE_MOVE_MP3,       m_resPool);
//  m_eater_die        = g.GetSMGR().GetSound(RES_EATER_DIE_MP3,        m_resPool);
//  m_move_slimmed     = g.GetSMGR().GetSound(RES_MOVE_SLIMMED_MP3,     m_resPool);
//  m_tutor_start      = g.GetSMGR().GetSound(RES_TUTOR_START_MP3,      m_resPool);
//
//
//  // messages
//  m_snd_go           = g.GetSMGR().GetSound(RES_GO_MP3,               m_resPool);
//  m_snd_bravo        = g.GetSMGR().GetSound(RES_BRAVO_MP3,            m_resPool);
//
//  // UI
//  m_panel_show       = g.GetSMGR().GetSound(RES_PANEL_SHOW_MP3,       m_resPool);
//  m_panel_hide       = g.GetSMGR().GetSound(RES_PANEL_HIDE_MP3,       m_resPool);
//  
//}
//
//GameResources::~GameResources() {
//  SAFE_DELETE(m_customMsg);
//}
//
//
//
//
//
//namespace msg_uids {
//  const u32 MSG_BRAVO               = 10;
//  const u32 MSG_START               = 11;
//  const u32 MSG_TUT                 = 12;
//}
//
//
//
//
//// ----- ScreenGameplay -------------------------------------------
//extern const mt::v2i16 g_margin(10, 10);
//
//ScreenGameplay::ScreenGameplay(Game& g)
//: m_game(g)
//, m_time(0.0f)
//, m_res(g)
//, m_suppressedTouchId(gBAD_ID)
//, m_gameLev(NULL)
//, m_isAutoScrollEnabled(false)
//, m_isPause(false)
//, m_gesturet(m_tracker)
//, m_messenger(m_res)
//, m_timeToSpark(2.0f)
//, m_soup(*m_res.m_soupSheet, m_res.m_grayvin )
//, m_ss(m_gesturet)
//, m_layout(NULL)
//, FadeScreen(1.35f)
//, m_fadeTarget(ftMax)
//, m_levelFinished(false)
//{
//#ifdef DBG_LEVEL
//  sLevIndx = DBG_LEVEL;
//#endif
//#ifdef DBG_LEVPAK
//  sLevPak = DBG_LEVPAK;
//#endif
//  g.GetRender().SetClearColor(gParams.SOUP_COLOR);
//  m_dragCell = gBadDragCell;
//  m_contDragPos = gBadDragCell;
//  m_nextAnimFrame = 0.0f;
//  m_moveAfterAutoFocusTime = 0.0f;
//
//  bacto::gProcessMusicManager(bacto::MUS_GAMEPLAY);
//
//  gCreateDecorVar1(m_soup.GetDecor(),*m_res.m_soupSheet);
//  m_messenger.SetListener(this);
//
//  /*--- create gui*/
//  {
//    const f32 replayButtonK     = 0.35f;
//    const f32 menuButtonK       = 0.45f;
//    const f32 stepBackK         = 0.55f;
//    const f32 stepForwK         = 0.35f;
//
//    m_onscreenButtons[BID_MENU]     = new bacto::Button(BID_MENU,  hud::but_menu,  m_res.m_hud, menuButtonK,  this, m_res.m_button_click);
//    m_onscreenButtons[BID_STEPB]    = new bacto::Button(BID_STEPB, hud::but_stepb, m_res.m_hud, stepBackK,    this, m_res.m_button_click);
//    m_onscreenButtons[BID_STEPF]    = new bacto::Button(BID_STEPF, hud::but_stepf ,m_res.m_hud, stepForwK,    this, m_res.m_button_click);
//    m_onscreenButtons[BID_REPLAY]   = new bacto::Button(BID_REPLAY,hud::but_replay,m_res.m_hud, replayButtonK,this, m_res.m_button_click);
//
//    m_layout = new gui::SidesLayout();
//
//
//    m_layout->SetFrame(r::Device::sContext.GetUserScreenSize() - g_margin * 2);
//    m_container.Add(m_layout, g_margin);
//    m_layout->Set(m_onscreenButtons[BID_MENU  ], gui::SidesLayout::sTopRight);
//    m_layout->Set(m_onscreenButtons[BID_REPLAY], gui::SidesLayout::sTopLeft);
//
//    gui::LinearLayout* plin = new gui::LinearLayout(gui::LinearLayout::dHorizontal, gui::LinearLayout::aEnd);
//    plin->Add(m_onscreenButtons[BID_STEPB], 0);
//    plin->Add(m_onscreenButtons[BID_STEPF], 0);
//    m_layout->Set(plin, gui::SidesLayout::sBottomLeft);
//
//    m_reporter = new g::Reporter(m_res.m_entIcos, m_res.m_fntBase24), v2i16(16/*g.GetRender().GetScreenSize().x/2*/,52);
//    //m_container.Add(m_reporter);
//
//#ifdef NEED_DEBUG_DEMO_BUTTON
//    m_onscreenButtons[BID_DBG_DEMO] = new bacto::Button(BID_DBG_DEMO, hud::but_demo,  m_res.m_hud, 0.3f, this, m_res.m_button_click);
//    m_container.Add(m_onscreenButtons[BID_DBG_DEMO],  mt::v2i16(r::Device::sContext.GetUserScreenSize().x-32,r::Device::sContext.GetUserScreenSize().y-32));
//#endif
//
//#ifdef NEED_DEBUG_NEXT_PREV_LEVEL_BUTTONS
//    const mt::v2u16 fullButtonSize = m_onscreenButtons[BID_STEPB]->GetSize();
//    const mt::v2u16 halfButtonSize = m_onscreenButtons[BID_REPLAY]->GetSize();
//    m_onscreenButtons[BID_DBG_NEXT] = new bacto::Button(BID_DBG_NEXT, hud::but_dbg_next,  m_res.m_hud, 0.3f, this, m_res.m_button_click);
//    m_onscreenButtons[BID_DBG_PREV] = new bacto::Button(BID_DBG_PREV, hud::but_dbg_prev, m_res.m_hud, 0.3f, this, m_res.m_button_click);
//    m_container.Add(m_onscreenButtons[BID_DBG_NEXT],  mt::v2i16(r::Device::sContext.GetUserScreenSize().x-32,r::Device::sContext.GetUserScreenSize().y-32));
//    m_container.Add(m_onscreenButtons[BID_DBG_PREV],  mt::v2i16(r::Device::sContext.GetUserScreenSize().x-32-(i16)32,r::Device::sContext.GetUserScreenSize().y-32));
//#endif
//  }  
//  m_onscreenButtons[BID_STEPF]->SetVisible(false);
//  LoadLevel();  
//}
//
//
//v2i16 ScreenGameplay::GetScreenPos(mt::v2i8 cell) const {
//  mt::v2f bpos( cell.x*gParams.CELL_WIDTH   + gParams.CELL_WIDTH / 2, cell.y*gParams.CELL_HEIGHT  + gParams.CELL_HEIGHT / 2);
//  return m_ss.BoardToScreen( bpos );
//}
//
//void ScreenGameplay::Input(const mt::v2i16& p, InputEvent::eAction type, int id) {
//  if(FadeScreen::ssActive != GetState())
//    return;
//
//  if( m_messenger.IsShowSomething() && (msg_uids::MSG_START != m_messenger.GetCurMsgUid()) ) {
//    m_tracker.Reset();
//    m_dragCell = gBadDragCell;    
//    if(type==InputEvent::iaDown)
//      m_messenger.touch();
//    return;
//  }
//
//  FadeScreen::Input(p, type, id);
//  m_tracker.Input(p, type, id);
//
//  // --- game input  
//  if(type==InputEvent::iaDown) {    
//    m_dragCell = gBadDragCell;
//    mt::v2f bordCoord = m_ss.ScreenToBoard(p);
//    mt::v2i16 cell = m_levHlp.GetCellIndex(bordCoord);
//    if(!m_gameLogic.IsDemoPlaing())
//    {
//      m_gameLogic.m_demoRecorder.RecordEvent(DemoRecorder::geTOUCH_DOWN,cell,0);
//    }
//
//    m_fieldEffects.Add(new FlyingParticle(g_rnd.frand()*5.0f+5.0f,bordCoord));
//
//    if(!m_gesturet.isAvailable(GestureTracker::egZoom) && m_levHlp.IsDragable(cell) && !m_gameLogic.IsDemoPlaing()) {
//      m_dragCell = cell;
//      m_startDragPos = GetScreenPos(cell);
//      m_contDragPos = p;
//      m_moveAfterAutoFocusTime = gParams.MOVE_AFTER_SELECT_ITEM_TIME;
//    }
//  }
//  if(type==InputEvent::iaUp) 
//    m_dragCell = gBadDragCell;    
//  if( type==InputEvent::iaRepeat && (m_dragCell!=gBadDragCell) )
//    m_contDragPos = p;  
//}
//
//void ScreenGameplay::Update(f32 dt) {
//  if(this != m_game.GetScreenManager().GetTop())
//    return;
//  FadeScreen::Update(dt);
//  m_reporter->Update(dt);
//  if(FadeScreen::ssActive != GetState() && FadeScreen::ssFadeIn != GetState())
//    return;
//  PROFILE_NODE(ScreenGameplay_update);
//  bool isCanMoveAfteAutofocus = false;
//
//  
//  m_moveAfterAutoFocusTime -= dt;
//
//  if(m_nextAnimFrame > 0.0f)
//  {
//    m_nextAnimFrame -= dt;
//    if(m_nextAnimFrame <= 0.0f)
//      m_levHlp.m_level->ClearMoveField();
//  }
//  bool isCanAnim = m_nextAnimFrame <= 0.0f;
//
//  if(m_moveAfterAutoFocusTime<0.0f) {
//    m_moveAfterAutoFocusTime=0;
//    isCanMoveAfteAutofocus = true;
//  }
//
//  if(m_dragCell==gBadDragCell) 
//    m_ss.Update(dt);
//  else 
//  {
//    m_startDragPos = GetScreenPos(m_dragCell);
//    mt::v2f movDir = m_contDragPos-m_startDragPos;
//    f32 scale = m_ss.GetScale();
//    if(movDir.length() > scale * gParams.CELL_HEIGHT * 3 / 4) {
//      f32 ang = NormalizeAngle(movDir.angle()- (f32)M_PI_2/2.0f);
//      i8 dir = (i8)(ang / M_PI_2);
//      if(isCanAnim && isCanMoveAfteAutofocus){
//        mt::v2i16 movedCell = m_gameLogic.Move(m_dragCell,dir);
//        if(m_dragCell!=movedCell)
//          m_onscreenButtons[BID_STEPB]->SetBlinking(false);
//        m_dragCell = movedCell;       
//        UpdateForwardStepButton();
//        m_nextAnimFrame = gParams.NEXT_MOVE_TIME;
//        if(gParams.NEXT_MOVE_TIME == 0.0f)
//        {
//          if(m_gameLogic.m_isLastMoveMerged)
//            m_nextAnimFrame = 0.45f; // если задержка между анимацией перемещения == 0 то при мерлде сразу происходит движение (проскок)
//
//          // if sync bacto move by steps
//          if( m_levHlp.IsSynchroBacto( m_levHlp.GetCellVal((i8)m_dragCell.x,(i8)m_dragCell.y) ) )
//            m_nextAnimFrame = 0.45f; 
//        }
//      }
//    }
//  }
//
//
//  // ----- update gfxes
//  m_soup.Update(dt);
//  m_timeToSpark -= dt;
//  if(m_timeToSpark<0)
//  {
//    m_timeToSpark = g_rnd.frand()*1.0f;
//    m_fieldEffects.Add(new FlyingParticle(g_rnd.frand()*5.0f+5.0f,mt::v2f(g_rnd.frand()*1000.0f,g_rnd.frand()*1000.0f)));
//  }
//  m_fieldEffects.Update(dt);
//
//  if(m_gameLogic.Update(dt) && !m_levelFinished ) {
//    m_levelFinished = true;
//    m_messenger.showCustom(m_res.m_customMsg->SwitchTo(GoBravoMessage::BRAVO),1.0f,Messenger::MMODE_SHOOT,2,L"",msg_uids::MSG_BRAVO);
//    m_res.m_snd_bravo->Play();
//    m_gameLogic.StopDemoPlayback();
//  }
//
//  // --- basic update
//  bacto::Button::Update(dt);
//  m_messenger.update(dt);
//  m_time += dt;
//}
//
//
//void ScreenGameplay::Clear() {
//  SAFE_DELETE(m_gameLev);
//}
//
//void ScreenGameplay::Untouch() {
//  m_dragCell = gBadDragCell;
//  m_tracker.KillTouches();
//}
//
//
//
//ScreenGameplay::~ScreenGameplay() {
//  Clear(); 
//  SAFE_DELETE(m_reporter);
//}
//
//void ScreenGameplay::Draw(r::Render& r) {
//  if(!m_gameLev)
//    return;
//  const mt::v2i16 uss = r::Device::sContext.GetUserScreenSize();
//
//
//#ifndef DBG_DISABLE_LEVEL_GFXES
//  m_soup.RenderBg(r,m_ss.GetOffset(), m_ss.GetScale());
//  m_gameLev->Render(r, m_ss.GetOffset(), m_ss.GetScale(),m_res, (mt::epsilon < gParams.NEXT_MOVE_TIME) ? (m_nextAnimFrame / gParams.NEXT_MOVE_TIME) : 0);
//  m_fieldEffects.DrawEffects(r,m_ss.GetOffset(), m_ss.GetScale(), m_res.m_soupSheet );
//  m_soup.RenderFg(r,m_ss.GetOffset(), m_ss.GetScale());
//#else
//  m_gameLev->Render(r, m_ss.GetOffset(), m_ss.GetScale(),m_res);
//#endif
//  
//  
//  // draw demo arrow
//  if(m_gameLogic.IsDemoPlaing()) {
//    v2f cell;
//    Direction dir;    
//    m_gameLogic.GetCurrentMove(cell,dir);
//    m_gameLev->RenderArrow(r, m_ss.GetOffset(), m_ss.GetScale(),cell,dir,m_gameLogic.m_hand,m_res);
//  }
//  // draw stretch line
////   if(m_dragCell!=gBadDragCell) {
////     v2f dir = (m_startDragPos-m_contDragPos);
////     if(dir.length()>16.0f) {
////       v2f scale = dir/128.0f;
////       dir.normalize();
////       m_res.m_gamePlay->Draw(r,gp::stretch_mat,m_startDragPos,COLOR_RED,mt::v2f(1.0f,1.0f),dir.angle()+(f32)M_PI_2,true);
////     }
////   }
//  
//
//#ifndef THUMBGEN_MODE
//  m_messenger.renderShading(r);
//  m_messenger.render(r);
//#endif  
//
//#ifndef THUMBGEN_MODE
//  FadeScreen::Draw(r);  
//#endif
//
//#ifdef THUMBGEN_MODE  
//  if(isNeedTakeScreenshot)
//  {
//    isNeedTakeScreenshot = false;
//    m_game.TakeScreenshot(sLevIndx,sLevPak);
//    gIncLevelIndex(sLevIndx); 
//    LoadLevel(sLevIndx);
//  }
//#endif
//  
//#ifdef NEED_DRAW_DEBUG_INPUT
//  DrawHelper dr(r);
//  if(m_dragCell!=gBadDragCell)
//    dr.DrawLine(m_startDragPos,m_contDragPos,COLOR_RED);
//  
//  for(u16 i = 0; i<m_tracker.GetNumTouches(); ++i) {
//   // m_ss.DbgDraw(r); // $$$Ink
//    const TouchTracker::Touch* tch = m_tracker.GetTouch(i);
//    dr.DrawCircle(tch->initial_pos,6,8,COLOR_ORANGE); 
//    dr.DrawLine(tch->pos,tch->initial_pos,COLOR_ORANGE);
//    dr.DrawCircle(tch->pos,8,8,COLOR_RED);      
//  }
//#endif
//#ifdef DBG_DRAW_LEVEL_INDEX
//  m_res.m_dbgText.Draw(r,v2f(0,0));
//#endif
//
//}
//
//
//void ScreenGameplay::HideIngameMenuWindow(bool /*playHideAnim*/) {  
//  m_onscreenButtons[BID_MENU]->SetVisible(true);
//  m_onscreenButtons[BID_STEPF]->SetVisible(true);
//  m_onscreenButtons[BID_STEPB]->SetVisible(true);
//  m_onscreenButtons[BID_REPLAY]->SetVisible(true);
//#ifdef NEED_DEBUG_DEMO_BUTTON
//  m_onscreenButtons[BID_DBG_DEMO]->SetVisible(true);
//#endif
//#ifdef NEED_DEBUG_NEXT_PREV_LEVEL_BUTTONS
//  m_onscreenButtons[BID_DBG_NEXT]->SetVisible(true);
//  m_onscreenButtons[BID_DBG_PREV]->SetVisible(true);
//  m_onscreenButtons[BID_DBG_DEMO]->SetVisible(true);
//#endif
//  m_isPause = false;
//  UpdateForwardStepButton();
//}
//
//void ScreenGameplay::ShowIngameMenuWindow() {
//  m_onscreenButtons[BID_MENU]->SetVisible(false);
//  m_onscreenButtons[BID_STEPF]->SetVisible(false);
//  m_onscreenButtons[BID_STEPB]->SetVisible(false);
//  m_onscreenButtons[BID_REPLAY]->SetVisible(false);
//#ifdef NEED_DEBUG_DEMO_BUTTON
//  m_onscreenButtons[BID_DBG_DEMO]->SetVisible(false);
//#endif
//#ifdef NEED_DEBUG_NEXT_PREV_LEVEL_BUTTONS
//  m_onscreenButtons[BID_DBG_NEXT]->SetVisible(false); 
//  m_onscreenButtons[BID_DBG_PREV]->SetVisible(false);
//  m_onscreenButtons[BID_DBG_DEMO]->SetVisible(false);
//#endif
//  m_isPause = true;
//  m_game.GetScreenManager().Push(new DlgIngameMenu(*this, m_game, 64));
//  m_tracker.KillTouches();
////m_ingameMenuPanel->GlowRestartButton(m_onscreenButtons[BID_MENU]->GetBlinking());  
//}
//
//void ScreenGameplay::GoToNextLevel()
//{
//  gIncLevelIndex(sLevIndx);
//  if(sLevIndx == (u32)(gParams.LEVELS_COUNT_IN_LEVEL_PAK) ) {
//    sLevIndx = 0;
//    if(sLevPak + 1 < gParams.LEVEL_PAK_COUNT)
//      ++sLevPak;
//    m_fadeTarget = ftMainMenu;
//    FadeOut(gParams.FADEOUT_TIME, mt::COLOR_WHITE);
//    return;
//  }
//  m_fadeTarget = ftNextLevel;
//  FadeOut(gParams.FADEOUT_TIME, g_fadeColor);
//}
//
//void ScreenGameplay::OnPressed( bacto::Button* pBtn ) {
//  EASSERT(pBtn);
//  if(m_messenger.IsShowSomething())  
//    return;
//  if(pBtn->m_id == BID_MENU) 
//    OnMenuPress();
//  else if(pBtn->m_id == BID_DBG_NEXT) {
//    GoToNextLevel();
//  }
//  else if(pBtn->m_id == BID_REPLAY) {
//    m_fadeTarget = ftReplayLevel;
//    FadeOut(gParams.FADEOUT_TIME, g_fadeColor);    
//  }
//  else if(pBtn->m_id == BID_DBG_PREV) {
//    gDecLevelIndex(sLevIndx);
//    LoadLevel(sLevIndx);
//  }
//  else if(pBtn->m_id == BID_STEPF) 
//    OnStepForward();
//  else if(pBtn->m_id == BID_STEPB) 
//    OnStepBack();
//  else if(pBtn->m_id == BID_DBG_DEMO) {    
//    ResetLevel(false);
//    m_gameLogic.PlayDemo();
//  }
//}
//
//void ScreenGameplay::OnMenuPress() {  
//  ShowIngameMenuWindow();
//}
//
//void ScreenGameplay::LoadLevel(u32 levelIndex) {
//  sLevIndx = levelIndex;
//  m_fadeTarget = ftNextLevel;
//  FadeOut(gParams.FADEOUT_TIME, g_fadeColor);
//}
//
//void ScreenGameplay::UpdateForwardStepButton() {
//  m_onscreenButtons[BID_STEPF]->SetVisible(!m_gameLogic.IsTopState());
//}
//
//void ScreenGameplay::OnStepForward() {
//  if(!m_gameLogic.IsDemoPlaing()) {
//    m_gameLogic.StepForward();    
//    UpdateForwardStepButton();
//  }
//}
//
//void ScreenGameplay::OnStepBack() {
//  if(!m_gameLogic.IsDemoPlaing() && !m_gameLogic.IsEating()) {
//    m_onscreenButtons[BID_REPLAY]->SetBlinking(false);
//    m_onscreenButtons[BID_STEPB]->SetBlinking(false);
//    g::Reporter::Message* msg = m_reporter->GetMessage(FAILED_MSG_UID);
//    if(msg)
//      msg->m_timeToDie = 1;
//    m_gameLogic.StepBackward();
//    UpdateForwardStepButton();
//  }
//}
//
//void ScreenGameplay::OnHideIngameMenuWindow() {
//  HideIngameMenuWindow(true);
//}
// 
//void ScreenGameplay::OnMessageFinish(u32 msgUid) {
//  if(msgUid==msg_uids::MSG_BRAVO)
//    GoToNextLevel();
//  else if(msgUid==msg_uids::MSG_TUT)
//  { // on end show tutorial
//    m_messenger.fadeout();
//    m_messenger.showCustom(m_res.m_customMsg->SwitchTo(GoBravoMessage::GO),1.0f,Messenger::MMODE_SHOOT,1.5f,L"",msg_uids::MSG_START);
//    m_res.m_snd_go->Play();
//    //m_messenger.show(m_res.m_popupGo, popup_go::egg,1.0f,Messenger::MMODE_SHOOT,1.5f,L"",msg_uids::MSG_START);
//  }
//  else if (msgUid==msg_uids::MSG_START) {
//  }
//}
//
//
//#define SHOW_TUTORIAL(iLEV,iLEVPAK,baseName) \
//  if((sLevIndx==iLEV) && (sLevPak==iLEVPAK)){\
//  m_game.GetScreenManager().Push(new DlgTutorial(&m_game.GetScreenManager(), m_game.GetRender(), #baseName));\
//  m_res.m_tutor_start->Play(); \
//}
//
//void ScreenGameplay::StartMessageChain() {
//  //m_messenger.show(m_res.m_popupGo, popup_go::egg,1.0f,Messenger::MMODE_SHOOT,1.5f,L"",msg_uids::MSG_START);
//  m_messenger.showCustom(m_res.m_customMsg->SwitchTo(GoBravoMessage::GO),1.0f,Messenger::MMODE_SHOOT,1.5f,L"",msg_uids::MSG_START);
//  SHOW_TUTORIAL( 0,0,tut_00);
//  SHOW_TUTORIAL( 1,0,tut_02);
//
////   SHOW_TUTORIAL( 0,0,tut_00);
////   SHOW_TUTORIAL( 2,0,tut_01);
////   SHOW_TUTORIAL( 4,0,tut_02);
////   SHOW_TUTORIAL( 8,0,tut_03);
////   SHOW_TUTORIAL( 9,0,tut_04);
////   SHOW_TUTORIAL(12,0,tut_05);
////   SHOW_TUTORIAL(15,0,tut_06);
//  //  m_res.m_snd_go->Play();
//}
//
//void ScreenGameplay::ResetLevel( bool /*isStartMesageChain*/ ) {
//  m_onscreenButtons[BID_REPLAY]->SetBlinking(false);
//  m_onscreenButtons[BID_STEPB]->SetBlinking(false);
//  g::Reporter::Message* msg = m_reporter->GetMessage(FAILED_MSG_UID);
//  if(msg)
//    msg->m_timeToDie = 0; // no smooth disappearing on level restart
//    
//  m_tracker.Reset();
//  m_dragCell = gBadDragCell;
//
//  SAFE_DELETE(m_gameLev);
//  m_gameLev = gLoadGameLevel(m_res,sLevIndx,sLevPak);
//  EASSERT(m_gameLev);
//
//  GameMap* gmap = m_gameLev->m_layers[1];
//  UpdatePedia(gmap);
//
//  CenterOnLevel(true);
//
//  m_levHlp.Init(m_gameLev);    
//  m_gameLogic.Init(&m_levHlp,this);  
//
//#ifdef DBG_DRAW_LEVEL_INDEX
//  m_res.m_dbgText.SetText(str::StrHelperW::ToString(sLevIndx).c_str()); // todo: strbuilder
//  m_res.m_dbgText.SetColor(COLOR_WHITE);
//#endif
//  UpdateForwardStepButton();
//}
//
//void ScreenGameplay::GlowRestart() {  
//  m_onscreenButtons[BID_REPLAY]->SetBlinking(true);
//  m_onscreenButtons[BID_STEPB]->SetBlinking(true);
//
// 
//  m_reporter->AddMessage( m_game.GetStrings().GetString(lsfail_level_msg),COLOR_WHITE,g::Reporter::s_infiniteShowTime, FAILED_MSG_UID);
//}
//
//void ScreenGameplay::OnScreenResized()
//{
//  m_layout->SetFrame(r::Device::sContext.GetUserScreenSize() - g_margin * 2);
//  CenterOnLevel(false);
//}
//
//void ScreenGameplay::CenterOnLevel(bool isntant)
//{
//  if(!m_gameLev)
//    return;
//  mt::v2i16 uss = r::Device::sContext.GetUserScreenSize();
//  Recti8    c   = m_gameLev->m_layers[0]->CalcRect();
//  mt::Rectf hfs = mt::Rectf(c.left, c.right + 1.0f, c.top, c.bottom + 1.0f).Scale(gParams.CELL_WIDTH);
//  hfs.Expand(gParams.CELL_WIDTH / 4);
//  m_ss.SetLimits(Min(uss.x / hfs.Width(), uss.y / hfs.Height()), hfs, isntant);
//}
//
//void ScreenGameplay::SpawnBubbleAtCell( mt::v2i8& cell ) {
//  mt::v2i16 pos = GetScreenPos(cell);
//  m_fieldEffects.Add(new FlyingParticle(g_rnd.frand()*5.0f+5.0f,m_ss.ScreenToBoard(pos)));
//}
//
//void ScreenGameplay::ExitToLevelSelect() {
//  m_fadeTarget = ftLevelSelect;
//  FadeOut(gParams.FADEOUT_TIME, mt::COLOR_WHITE);
//}
//
//void ScreenGameplay::onFadedIn ()
//{
//  if(!m_gameLogic.IsDemoPlaing())
//    StartMessageChain();
//}
//
//void ScreenGameplay::onFadedOut()
//{
//  HideIngameMenuWindow(false);
//  if(ftMainMenu == m_fadeTarget) {
//    while(this != m_game.GetScreenManager().GetTop())
//      m_game.GetScreenManager().Pop(); // close all dialogs
//    m_game.GetScreenManager().Pop(this);
//    m_game.GetScreenManager().Push(new ScreenMainMenu(m_game));
//  }
//  else if(ftLevelSelect == m_fadeTarget) {
//    while(this != m_game.GetScreenManager().GetTop())
//      m_game.GetScreenManager().Pop(); // close all dialogs
//    m_game.GetScreenManager().Pop(this);
//    m_game.GetScreenManager().Push(new ScreenLevelSelect(m_game));
//  }
//  else if(ftNextLevel == m_fadeTarget){
//    LoadLevel();
//    FadeIn(gParams.FADEIN_TIME, g_fadeColor);
//  }
//  else if(ftReplayLevel == m_fadeTarget){
//    ResetLevel(true);
//    FadeIn(gParams.FADEIN_TIME, g_fadeColor);
//  }
//  else {
//    EALWAYS_ASSERT(L"Wrong fade target in ScreenGameplay");
//  }
//}
//
//void ScreenGameplay::LoadLevel()
//{
//  // reset input
//  m_dragCell = gBadDragCell;
//  m_startDragPos = gBadDragCell;
//  m_contDragPos = gBadDragCell;
//  m_tracker.KillTouches();
//  m_tracker.Reset();
//
//  isNeedTakeScreenshot = true;
//  m_levelFinished = false;
//  ResetLevel(true);
//
//  mt::v2i8 c = m_gameLev->m_layers[0]->CalcCenter();
//  v2f fsize( c.x*gParams.CELL_WIDTH,c.y*gParams.CELL_HEIGHT);
//  // m_decor.SetPos(fsize); // todo:
//  m_soup.Regen();
//}
//
//
//
//
//void ScreenGameplay::UpdatePedia( GameMap* gmap )
//{  
//  for( i8 y=0;y<GameMap::sMAX_HEIGHT;++y)
//    for( i8 x=0;x<GameMap::sMAX_WIDTH;++x) 
//    {
//      u8 val = gmap->GetVal(x,y);
//      if(val!=cvRestricted) 
//      {
//        if(m_levHlp.IsBoomBacto(val))         CheckPedia("bacto_boom");
//        else if(m_levHlp.IsClueBacto(val))    CheckPedia("bacto_clue");
//        else if(m_levHlp.IsCommonBacto(val))  CheckPedia("bacto_common");
//        else if(m_levHlp.IsDirtyBacto(val))   CheckPedia("bacto_dirty");
//        else if(m_levHlp.IsFillerBacto(val))  CheckPedia("bacto_filler");
//        else if(m_levHlp.IsGrowBacto(val))    CheckPedia("bacto_grow");
//        else if(m_levHlp.IsJBacto(val))       CheckPedia("bacto_jumper");
//        else if(m_levHlp.IsSynchroBacto(val)) CheckPedia("bacto_synchro");
//        else if(m_levHlp.IsCmnBlock(val))     CheckPedia("block");
//        else if(m_levHlp.IsEater(val))        CheckPedia("eater");
//        else if(val==cvFiller)                CheckPedia("filler");
//        else if(m_levHlp.IsFood(val))         CheckPedia("food");
//        else if(m_levHlp.IsSnakeBody(val))    CheckPedia("snake");
//        else if(val==cvCommonWall)            CheckPedia("wall_cmn");
//        else if(val==cvGodWall)               CheckPedia("wall_god");
//
//      }
//    }
//}
//
//void ScreenGameplay::CheckPedia( const std::string& ent )
//{
//  bacto::Pedia& pedia = m_game.GetPedia();
//  if(pedia.IsPageLocked(ent))
//  {
//    pedia.UnlockPage(ent);
//    m_reporter->AddImageMessage(ent,COLOR_WHITE,L"you unlock new entity. Check bactopedia.",g::Reporter::s_defShowTime,0);
//  }
//}
