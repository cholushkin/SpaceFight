#include "screenmainmenu.h"
#include "screengameplay.h"
#include "res/sheet_mainmenu.h"
#include "core/render/r_render.h"
#include "res/resources.h"
#include "windows.h"
#include "config.h"

using namespace mt;
using namespace in;
using namespace r;

ScreenMainMenu::ScreenMainMenu(Application& g)
    : m_app(g)
    , m_fnt(GetFont(RES_8BIT_FNT, g.GetRPool(), g.GetRender()))
    , m_sheetMainMenu(mainmenu::GetSprite(g.GetRender(), g.GetRPool()))
    , m_controllerSelector(m_sheetMainMenu)
{
    g.GetRender().SetClearColor(COLOR_BLACK);
    m_logoText.SetText(m_fnt, L"Space Fight");
    m_logoText.SetFontScale(4);
    m_logoText.SetAlign(BitmapText::tlCenter);

    m_promtText.SetText(m_fnt, L"Hit <ENTER> to start the fight");
    m_promtText.SetAlign(BitmapText::tlCenter);
    m_promtText.SetPosition(v2f(0,700));    
    
    //m_app.GetSMGR().StartMusic(RES_MUSMAINMENU_MP3);
}

ScreenMainMenu::~ScreenMainMenu()
{
    m_app.GetSMGR().StopMusic();
}

void ScreenMainMenu::Draw(r::Render& r)
{
    // draw background
    const auto logoTextPos = v2f(r.GetScreenSize().x/2.0f, 0.0f);
    m_sheetMainMenu->Draw(r, mainmenu::MainMenuBackground, r.GetScreenSize()/2, COLOR_WHITE);

    // draw wifget
    m_controllerSelector.Draw(r, v2f());

    // draw logo text
    m_logoText.Draw(r, logoTextPos);

    // draw promttext
    m_promtText.Draw(r, logoTextPos);
    
}

void ScreenMainMenu::Update(f32 /*dt*/)
{
    if (GetKeyState(VK_RETURN) < 0)
    {
        m_app.GetScreenManager().Pop();
        m_app.GetScreenManager().Push(new ScreenGameplay(m_app, ScreenGameplay::SessionContext()));
    }
}


