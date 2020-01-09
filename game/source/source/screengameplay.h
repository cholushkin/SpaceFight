#ifndef screengameplay_h__
#define screengameplay_h__
//#include "config.h"
#include "application.h"
#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "ext/primitives/r_particles.h"
#include "inputsystem.h"
#include "rendersystem.h"
#include "physicssystem.h"
#include "playercontrollersystem.h"

#include "widgets.h"

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
    // can be moved to ase class
    snd::Sound* Sound(int resId) {
        return m_app.GetSMGR().GetSound(resId, m_resPool);
    }
private:
    Application&                m_app;
    res::ResourcesPool          m_resPool;
};

class LevelCreator final
{
    DENY_COPY(LevelCreator)
private:
    class GenerationOptions
    {
    public:
        GenerationOptions();
        mt::v2i16 PlanetsAmmount;
        mt::v2i16 EnergyStationAmmount;
        mt::v2i16 AsteroidsAmmount;
    };

public:
    LevelCreator(entt::DefaultRegistry& registry, PhysicsSystem& psx);
    void CreateLevelRandom();
    void DeleteEntity(uint32_t&);

private:
    entt::DefaultRegistry&      m_registry;
    PhysicsSystem&              m_physicsSystem;
    GenerationOptions           m_options;
    std::vector<mt::v2f>        m_created;


    void CreatePlayerEntity(int playerID, const mt::v2f& pos);
    void CreatePlanet(const mt::v2f& pos);
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
    LevelCreator            m_levelCreator;
    InputSystem             m_inputSystem;
    RenderSystem            m_renderSystem;
    PhysicsSystem           m_physicsSystem;
    PlayerControllerSystem  m_playerControllerSystem;
    

    WidgetPlayerDashboard   m_player1Dashboard;
    WidgetPlayerDashboard   m_player2Dashboard;
    WidgetModalMessage      m_modalMessenger;

    
};

#endif // screengameplay_h__
