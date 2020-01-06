#include "screenmainmenu.h"
#include "screengameplay.h"
#include "core/render/r_render.h"
#include "res/resources.h"
#include "res/sheet_mainmenu.h"

using namespace mt;
using namespace in;
using namespace r;

ScreenMainMenu::ScreenMainMenu(Application& g)
    : m_app(g)
    , m_fnt(GetFont(RES_8BIT_FNT, g.GetRPool(), g.GetRender()))
    , m_sheetMainMenu(mainmenu::GetSprite(g.GetRender(), g.GetRPool()))
{
    g.GetRender().SetClearColor(COLOR_BLACK);
    m_logoText.SetText(m_fnt, L"Space Fight");
    m_logoText.SetFontScale(4);
    m_logoText.SetAlign(BitmapText::tlCenter);

    m_app.GetSMGR().StartMusic(RES_MUSMAINMENU_MP3);
    //m_app.GetSMGR().P(RES_TRACKMAINMENU_MOD);

    /*m_trail.m.m_filter = Material::fmNearest;
    return LoadPS(m_trail, RES_FIRE_PSI, RES_EXPLOSION_PNG, r, p);*/

}

void ScreenMainMenu::Draw(r::Render& r)
{
    // draw background
    const auto logoTextPos = v2f(r.GetScreenSize().x/2.0f, 0.0f);
    m_sheetMainMenu->Draw(r, mainmenu::MainMenuBackground, r.GetScreenSize()/2, COLOR_WHITE);

    // draw logo text
    m_logoText.Draw(r, logoTextPos);
    //    const mt::v2i16& s = r::Device::sContext.GetUserScreenSize();
    //    // white bg
    //    const mt::v2f bgcrds[4] = { mt::v2f(0, 0), mt::v2f(s.x, 0), mt::v2f(s.x, s.y), mt::v2f(0, s.y) };
    //    r.PostQuad(bgcrds, r::g_def_uv, r::g_def_colors, r::Material());
    //    // table
    //    const mt::v2f coords[4] = { mt::v2f(0, s.y * 3.0f / 5.0f), mt::v2f(s.x, s.y * 3.0f / 5.0f), mt::v2f(s.x, s.y), mt::v2f(0, s.y) };
    //    const     u32 colors[4] = { SETA(mt::COLOR_BLACK, 64), SETA(mt::COLOR_BLACK, 64), SETA(mt::COLOR_WHITE, 0), SETA(mt::COLOR_WHITE, 0) };
    //    r.PostQuad(coords, r::g_def_uv, colors, r::Material());
    //    FadeScreen::Draw(r);
    //#ifdef SHOW_VERSION_IN_MAIN_MENU
    //    m_res.m_text.SetText(INTERNAL_GAME_VERSION);
    //    m_res.m_text.Draw(r, v2f(2.0f, r::Device::sContext.GetUserScreenSize().y - 25.0f));
    //#endif
}

void ScreenMainMenu::Update(f32 /*dt*/)
{

}

void ScreenMainMenu::Input(const mt::v2i16& /*p*/, InputEvent::eAction /*type*/, int /*id*/)
{
}

