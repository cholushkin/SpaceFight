//#ifndef screengameplay_h__
//#define screengameplay_h__
//#include "config.h"
//#include "game.h"
//#include "ext/gui/gui_base.h"
//#include "ext/primitives/r_primitives.h"
//#include "ext/primitives/r_sheet_sprite.h"
//#include "gamelevel.h"
//#include "tileset.h"
//#include "button.h"
//#include "ingamemenu.h"
//#include "screenstate.h"
//#include "ext/input/in_touchtracker.h"
//#include "decor.h"
//#include "messenger.h"
//#include "geffect.h"
//#include "g_reporter.h"
//
//#include "rect_shader.h"
//#include "gesturetracker.h"
//#include "viewtransform.h"
//#include "gfx_soup.h"
//#include "screen_fader.h"
//
//struct GameResources {
//  enum eSheets {
//    sBacto = 0,
//    sBlock = 1,
//    sTiles = 2,
//    sSnake = 3,
//    sMax
//  };
//  GameResources(Game& g);
//  ~GameResources();
//  r::SheetSprite*               m_popupGo;
//  r::SheetSprite*               m_gameTiles[sMax];
//  r::SheetSprite*               m_bactos;
//  r::SheetSprite*               m_hud;      
//  r::SheetSprite*               m_entIcos;      
//  Tileset                       m_gameTileset;
//  r::SheetSprite*               m_soupSheet;
//  r::Material                   m_grayvin;
//  res::ResourcesPool            m_messengerResPool;
//  GoBravoMessage*               m_customMsg;
//  const r::BitmapFont*          m_fntBase32;
//  const r::BitmapFont*          m_fntBase24;
//#ifdef DBG_DRAW_LEVEL_INDEX  
//  r::BitmapText                 m_dbgText;
//#endif
//  // sounds 
//  snd::Sound* m_bacto_connect;
//  snd::Sound* m_block_move;
//  snd::Sound* m_snake_move;
//  snd::Sound* m_eater_die;
//  snd::Sound* m_move_slimmed;
//  snd::Sound* m_tutor_start;
//
//  snd::Sound* m_button_click;
//  snd::Sound* m_boom;
//  snd::Sound* m_eat_food;
//  snd::Sound* m_eater_feed_end;
//  snd::Sound* m_eater_feed_start;
//  snd::Sound* m_eater_feed_step;
//  snd::Sound* m_grow_step;
//  snd::Sound* m_move_eater_start;
//  snd::Sound* m_move_eater_step;
//  snd::Sound* m_move_stuff;
//  snd::Sound* m_snake_eat;
//  snd::Sound* m_snd_go;
//  snd::Sound* m_snd_bravo;
//  snd::Sound* m_panel_show;
//  snd::Sound* m_panel_hide;
//
//  res::ResourcesPool& GetPool() { return m_resPool; }
//private:
//  res::ResourcesPool            m_resPool;  
//};
//
//class ScreenGameplay
//: public FadeScreen
//, public bacto::Button::IListener
//, public Messenger::IMessengerListener
//, public OnResizeListener
//{
//  DENY_COPY(ScreenGameplay)
//  enum EnButtonIds {
//    BID_MENU      = 0,
//    BID_STEPB     = 1,
//    BID_STEPF     = 2,
//    BID_REPLAY    = 3,
//    BID_DBG_PREV  = 4,
//    BID_DBG_NEXT  = 5,
//    BID_DBG_DEMO  = 6,    
//    BID_COUNT
//  };
//
//public:
//  GameResources            m_res;
//  GestureTracker           m_gesturet;
//  in::TouchTracker         m_tracker;
//  bool                     m_isPause;
//
//  // from OnResizeListener
//  virtual void OnScreenResized();
//
//public:
//  ScreenGameplay(Game& g);
//  ~ScreenGameplay();
// 
//  virtual void Draw(r::Render& r);
//  virtual void Update(f32 /*dt*/);
//
//  void LoadLevel();
//
//  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id);
//  gui::ContainerAbsolute&  GetContainer(){ return m_container; }
//  Game&  GetGame() { return m_game; }
//  void Untouch();
//
//  void GlowRestart();
//  void SpawnBubbleAtCell( mt::v2i8& cell );
//
//
//  static u32                sLevIndx;
//  static u32                sLevPak;
//protected:
//  virtual void onFadedIn ();
//  virtual void onFadedOut();
//private:
//  bool                      m_levelFinished;
//  EffectManager             m_fieldEffects;
//  f32                       m_timeToSpark;
//  ViewTransformTracker      m_ss;
//  f32                       m_nextAnimFrame;
//  f32                       m_time;  
//  f32                       m_moveAfterAutoFocusTime;
//  Game&                     m_game;
//  int                       m_suppressedTouchId;
//  GameLevel*                m_gameLev;
//  LevelHelper               m_levHlp;
//  GameLogic                 m_gameLogic;
//  mt::v2i16                 m_dragCell;
//
//  mt::v2i16                 m_startDragPos;
//  mt::v2i16                 m_contDragPos;
//
//  bool                      m_isAutoScrollEnabled;
//
//  bacto::Button*            m_onscreenButtons[BID_COUNT];
//  gui::SidesLayout*         m_layout;
//  Messenger                 m_messenger;
//  bacto::GfxSoup            m_soup;
//  g::Reporter*              m_reporter;
//
//private:
//  void CenterOnLevel(bool isntant);
//  void Clear();  
//  mt::v2i16 GetScreenPos(mt::v2i8 cell) const;
//  virtual void OnPressed(bacto::Button* pBtn);
//  virtual void OnMessageFinish(u32 msgUid);
//  void StartMessageChain();
//  void HideIngameMenuWindow(bool playHideAnim);
//  void ShowIngameMenuWindow();  
//
//public: // hud button event handlers
//  void LoadLevel(u32 levelIndex);
//  void ResetLevel(bool isStartMesageChain);
//  void GoToNextLevel();
//
//  void OnMenuPress();
//  void UpdateForwardStepButton();  
//  void OnStepForward();
//  void OnStepBack();  
//  void OnHideIngameMenuWindow();
//
//  void ExitToLevelSelect();
//
//
//private:
//  void UpdatePedia( GameMap* gmap );
//  void CheckPedia( const std::string& ent );
//
//private:
//  enum FadeTarget
//  {
//    ftMainMenu    = 0,
//    ftLevelSelect = 1,
//    ftNextLevel   = 2,
//    ftReplayLevel = 3,
//    ftMax         = 4,
//  };
//
//  FadeTarget m_fadeTarget;
//};
//
//#endif // screengameplay_h__
