#ifndef screengameplay_h__
#define screengameplay_h__
//#include "config.h"
#include "application.h"
#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "inputsystem.h"
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

struct GameResources
{
    GameResources(Application& app);

    r::SheetSprite*             m_sheet;
    const r::BitmapFont*        m_fnt;
    snd::Sound*                 m_sfxExplode1;
    snd::Sound*                 m_sfxExplode2;
    snd::Sound*                 m_sfxExplode3;
    snd::Sound*                 m_sfxExplodeHit;
    snd::Sound*                 m_sfxMenuApply;
    snd::Sound*                 m_sfxMenuSelect;
    snd::Sound*                 m_sfxPlasmaHit;
    snd::Sound*                 m_sfxRefillEnergyStep;
    snd::Sound*                 m_sfxShoot;
private:
    Application&                m_app;
    res::ResourcesPool          m_resPool;
};

class ScreenGameplay : public gui::BasicScreen
{
    DENY_COPY(ScreenGameplay)

    enum EnGameStates
    {
        Playing = 0,
        Paused = 1,
    };

public:
    ScreenGameplay(Application& app);
    ~ScreenGameplay();

    void Draw(r::Render& r);
    void Update(f32 /*dt*/);


private:
    Application&            m_app;
    GameResources           m_res;
    EnGameStates            m_gameState;
    float                   m_time;

    InputSystem             m_inputSystem;
    entt::DefaultRegistry   m_registry;


    void CreatePlayerEntity();
};

#endif // screengameplay_h__
