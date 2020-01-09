#ifndef screengameplay_h__
#define screengameplay_h__

#include "application.h"
#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "ext/primitives/r_particles.h"
#include "inputsystem.h"
#include "rendersystem.h"
#include "physicssystem.h"
#include "gamerulessystem.h"
#include "playercontrollersystem.h"
#include "level.h"
#include "widgets.h"

struct GameResources
{
    GameResources(Application& app);

    const r::SheetSprite*       m_sheet;
    const r::BitmapFont*        m_fnt;
    snd::Sound*                 m_sfxExplode1;
    snd::Sound*                 m_sfxExplode2;
    snd::Sound*                 m_sfxExplode3;
    snd::Sound*                 m_sfxHit;
    snd::Sound*                 m_sfxMenuApply;
    snd::Sound*                 m_sfxMenuSelect;
    snd::Sound*                 m_sfxPlasmaHit;
    snd::Sound*                 m_sfxRefillEnergyStep;
    snd::Sound*                 m_sfxShoot;
    r::ParticleSystemInfo       m_particleSysExplosionA;
    r::ParticleSystemInfo       m_particleSysExplosionB;
    r::ParticleSystemInfo       m_particleSysTrail;
protected:
    snd::Sound* Sound(int resId) {
        return m_app.GetSMGR().GetSound(resId, m_resPool);
    }
private:
    Application&                m_app;
    res::ResourcesPool          m_resPool;
};

class ScreenGameplay : public gui::BasicScreen
{
    DENY_COPY(ScreenGameplay)

    enum EnGameStates
    {
        Initialization = 0,
        Playing = 1,
        Message = 2,
        Paused = 3,
    };
public:
    //class GameContext
    //{ 
    //public:
    //    GameContext(ScreenGameplay* screenGameplay);
    //    GameResources&           m_res;
    //    Level&                   m_level;
    //    InputSystem&             m_inputSystem;
    //    RenderSystem&            m_renderSystem;
    //    PhysicsSystem&           m_physicsSystem;
    //    PlayerControllerSystem&  m_playerControllerSystem;
    //    entt::DefaultRegistry&   m_registry;
    //};

public:
    ScreenGameplay(Application& app);
    ~ScreenGameplay();

    void Draw(r::Render& r);
    void Update(f32 /*dt*/);
    void SetState(EnGameStates state);


private:
    Application&            m_app;
    GameResources           m_res;
    EnGameStates            m_gameState;
    float                   m_time;

    entt::DefaultRegistry   m_registry;
    Level                   m_level;
    InputSystem             m_inputSystem;
    GameRulesSystem         m_gameRuleSystem;
    RenderSystem            m_renderSystem;
    PhysicsSystem           m_physicsSystem;
    PlayerControllerSystem  m_playerControllerSystem;
    

    WidgetPlayerDashboard   m_player1Dashboard;
    WidgetPlayerDashboard   m_player2Dashboard;
    WidgetModalMessage      m_modalMessenger;
    //GameContext             m_gameContext;
    
};

#endif // screengameplay_h__
