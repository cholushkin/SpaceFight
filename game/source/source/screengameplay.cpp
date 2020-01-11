#include "screengameplay.h"

#include "config.h"
#include "PhysicsAgentComponent.h"
#include "PhysicsObstacleComponent.h"
#include "widgets.h"
#include "ext/strings/str_base.h"
#include "screenmainmenu.h"

// --- res
#include "res/resources.h"
#include "res/sheet_gameplay.h"


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
    m_explSprite = ani_expl::CreateSprite(app.GetRender(), app.GetRPool());
    m_fnt = GetFont(RES_8BIT_FNT, app.GetRPool(), app.GetRender());

    // sounds
    m_sfxHit = Sound(RES_SFXHIT_WAV);
    m_sfxShoot = Sound(RES_SFXSHOOT_WAV);
    m_sfxExplode1 = Sound(RES_SFXEXPLODE1_WAV);
    m_sfxExplode2 = Sound(RES_SFXEXPLODE2_WAV);
    m_sfxExplode3 = Sound(RES_SFXEXPLODE3_WAV);
    m_sfxPlasmaHit = Sound(RES_SFXPLASMAHIT_WAV);
    m_sfxMenuApply = Sound(RES_SFXMENUAPPLY_WAV);
    m_sfxMenuSelect = Sound(RES_SFXMENUSELECT_WAV);
    m_sfxRefillEnergyStep = Sound(RES_SFXREFILLENERGYSTEP_WAV);

    // particles systems
    LoadPS(m_particleSysExplosionA, RES_EXPLOSION1_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
    LoadPS(m_particleSysExplosionB, RES_EXPLOSION2_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
    LoadPS(m_particleSysTrail, RES_BURST_PSI, RES_PARTICLES_PNG, app.GetRender(), m_resPool);
    
}

// ScreenGameplay 
// ---------------------------------------------------------------
ScreenGameplay::ScreenGameplay(Application& app, SessionContext sessionState)
    : m_app(app)
    , m_time(0.0f)
    , m_res(app)
    , m_level(m_registry, m_physicsSystem, m_sessionContext)
    , m_player1Dashboard(m_registry, m_res)
    , m_player2Dashboard(m_registry, m_res)
    , m_modalMessenger(m_registry, m_res)
    , m_playerControllerSystem(m_inputSystem, m_level)
    , m_gameRuleSystem(m_level)
    , m_physicsSystem(m_registry, m_level)
    , m_sessionContext(sessionState)
    , m_winConditionDelay(WIN_CONDITION_DELAY)
{
    SetState(Initialization);
}

ScreenGameplay::~ScreenGameplay()
{
}

void ScreenGameplay::Draw(r::Render& r)
{
    m_renderSystem.Render(r, m_res, m_registry);
    m_player1Dashboard.Draw(r, v2f(16, 16));
    m_player2Dashboard.Draw(r, v2f(16, 64 + 16));    
    m_modalMessenger.Draw(r, v2f());
}

void ScreenGameplay::Update(f32 dt)
{
    m_time += dt;

    m_inputSystem.Update(dt, m_registry);    

    // ----- update state
    if (m_gameState == MessageFight)
    {
        m_modalMessenger.Update(dt);
        if (m_modalMessenger.GetState() == WidgetModalMessage::Closed)
            SetState(Playing);
    }
    if (m_gameState == Playing)
    {
        m_renderSystem.Update(dt, m_registry);

        // update gameplay gui
        m_player1Dashboard.Update(dt);
        m_player2Dashboard.Update(dt);

        // update systems        
        m_playerControllerSystem.Update(dt, m_registry);
        m_physicsSystem.Update(dt, m_registry);
        m_gameRuleSystem.Update(dt, m_registry);
        if (m_gameRuleSystem.HasWinner())
        {
            m_winConditionDelay -= dt;
            if (m_winConditionDelay < 0.0f)
            {
                ++m_sessionContext.m_winCount[m_gameRuleSystem.GetWinnerID()];
                if (m_sessionContext.m_winCount[m_gameRuleSystem.GetWinnerID()] >= WIN_COUNT)
                    SetState(MessageFinalWin);
                else
                    SetState(MessageWin);
            }
        }
        else
        {
            if( m_inputSystem.GetCancelAction() )
                SetState(MessagePause);
        }
    }
    else if (m_gameState == MessageWin)
    {
        m_modalMessenger.Update(dt);
        if (m_modalMessenger.GetState() == WidgetModalMessage::Closed)
            StartNextRound();
    }
    else if (m_gameState == MessageFinalWin)
    {
        m_modalMessenger.Update(dt);
        if (m_inputSystem.GetCancelAction() && m_modalMessenger.GetState() == WidgetModalMessage::Static)
            ReturnToMainMenu();
    }
    else if (m_gameState == MessagePause)
    {
        m_modalMessenger.Update(dt);
        if (m_inputSystem.GetApplyAction())
            m_modalMessenger.Close();
        else if (m_inputSystem.GetCancelAction() && m_modalMessenger.GetState()== WidgetModalMessage::Static)
            ReturnToMainMenu();
        if (m_modalMessenger.GetState() == WidgetModalMessage::Closed)
            SetState(Playing);
    }
}

void ScreenGameplay::SetState(EnGameStates state)
{
    m_gameState = state;

    // ------ on enter state // todo: OnEnterState
    if (m_gameState == Initialization)
    {
        // create new level
        m_level.CreateLevelRandom();

        // assign widget to entity
        m_player1Dashboard.SetPlayer(0);
        m_player2Dashboard.SetPlayer(1);

        m_modalMessenger.Show(
            L"Fight!", 
            str::StringBuilderW()(L"Round: %0", m_sessionContext.m_round + 1), GUI_STD_SHOW_DURATION);

        SetState(MessageFight);
    }
    else if (m_gameState == MessageWin)
    {
        m_modalMessenger.Show(            
            str::StringBuilderW()(L"Player %0 wins", m_gameRuleSystem.GetWinnerID() + 1),
            L"",
            GUI_STD_SHOW_DURATION
        );
    }
    else if (m_gameState == MessageFinalWin)
    {
        m_modalMessenger.Show(
            str::StringBuilderW()(L"Player %0 wins in %1 rounds", m_gameRuleSystem.GetWinnerID() + 1, m_sessionContext.m_round + 1),
            L"Press <esc> to finish the battle"
        );
    }
    else if (m_gameState == MessagePause)
    {
        m_modalMessenger.Show(
            L"Pause",
            L"Press <enter> to resume or <esc> to quit"
        );
    }
}

void ScreenGameplay::ReturnToMainMenu()
{
    m_app.GetScreenManager().Pop();
    m_app.GetScreenManager().Push(new ScreenMainMenu(m_app));
}

void ScreenGameplay::StartNextRound()
{
    m_sessionContext.m_round++;
    m_app.GetScreenManager().Pop();
    auto screen = new ScreenGameplay(m_app, m_sessionContext);
    m_app.GetScreenManager().Push(screen);
}
