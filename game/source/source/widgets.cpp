#include "widgets.h"
#include "res/sheet_mainmenu.h"
#include "ext/draw2d/draw_helper.h"
#include "erps/erps.h"
#include "res/sheet_gameplay.h"
#include "config.h"
#include "screengameplay.h"
#include "ext/strings/str_base.h"

using namespace mt;
using namespace in;
using namespace r;
using namespace erps;


// WidgetControllerSelector
// --------------------------------------------------------------------------
WidgetControllerSelector::WidgetControllerSelector(const r::SheetSprite* spr)
    : m_sheetSprite(spr)
{
}

void WidgetControllerSelector::Update(f32 /*dt*/)
{

}

void WidgetControllerSelector::Draw(r::Render& r, const mt::v2f& /*origin*/)
{
    static const float offsetVert = 10.0f;
    static const float offsetHoriz = 10.0f;

    m_sheetSprite->Draw(r, mainmenu::IconControllerKeyboardLeftPart, v2f(100, 100), COLOR_WHITE);
    m_sheetSprite->Draw(r, mainmenu::LabelP1, v2f(180, 100), COLOR_WHITE);

    m_sheetSprite->Draw(r, mainmenu::IconControllerKeyboardRightPart, v2f(100, 200), COLOR_WHITE);
    m_sheetSprite->Draw(r, mainmenu::LabelP2, v2f(180, 200), COLOR_WHITE);
}

// WidgetPlayerDashboard
// --------------------------------------------------------------------------
WidgetPlayerDashboard::WidgetPlayerDashboard(entt::DefaultRegistry& registry, const GameResources& res)
    : m_res(res)
    , m_registry(registry)
    , m_time(0.0f)
{
    m_text.SetAlign(BitmapText::tlCenter);
}

void WidgetPlayerDashboard::Update(f32 dt)
{
    //m_text.SetFontScale(4);


    m_time += dt;
    if (m_time > GUI_APPEAR_DURATION)
        return;
    m_appearProgress = Clamp(m_time / GUI_APPEAR_DURATION, 0.0f, 1.0f);
    m_appearOffset = erps::GetErp(ERP_BOUNCE)(m_appearProgress) * 100 - 100;
}

void WidgetPlayerDashboard::SetPlayer(const uint32_t& ent)
{
    m_playerID = ent;
}

void WidgetPlayerDashboard::Draw(r::Render& r, const mt::v2f& origin)
{
    static auto GetPlayerLabelSprite = [](auto playerID) { return playerID == 0 ? gameplay::LabelP1 : gameplay::LabelP2; };

    DrawHelper dr(r);

    auto pivot = origin;
    pivot.x = origin.x + m_appearOffset;
    auto playerComp = m_registry.get<PlayerComponent>(m_playerID);

    // draw background panel
    dr.FillRect(Rectf(pivot.x, pivot.x + 100, pivot.y, pivot.y + 40), COLOR_BEIGE);

    // draw energy
    auto length = playerComp.m_energy / SHIP_ENERGY_MAX;
    m_text.SetText(m_res.m_fnt, str::StringBuilderW()(L"energy:%0", static_cast<int>(playerComp.m_energy)));
    dr.FillRect(Rectf(pivot.x, pivot.x + 100 * length, pivot.y + 10, pivot.y + 20 + 10), COLOR_ORANGE);
    m_text.Draw(r, pivot + v2f(50, 10));

    // draw victories
    for (int i = 0; i < playerComp.m_winCount; ++i)
        m_res.m_sheet->Draw(r, gameplay::LabelVictory, pivot + v2f(34.0f + 22.0f*i, 0), COLOR_WHEAT);

    // draw player label
    m_res.m_sheet->Draw(r, GetPlayerLabelSprite(playerComp.m_playerID), pivot);
}


// WidgetPlayerDashboard
// -------------------------------------------------------------
WidgetModalMessage::WidgetModalMessage(entt::DefaultRegistry& registry, const GameResources& res)
    : m_res(res)
    , m_registry(registry)
    , m_time(0.0f)
{
    m_textMain.SetAlign(BitmapText::tlCenter);
    m_textSecondary.SetAlign(BitmapText::tlCenter);
    SetState(VIEW_CLOSED);
}

void WidgetModalMessage::SetState(ViewMode mode)
{
    m_viewMode = mode;
    m_viewSubstate = Appearing;
    m_time = 0.0f;
    m_progress = 0.0f;

    if (mode == VIEW_FIGHT) // todo: state machine
    {
        m_textMain.SetText(m_res.m_fnt, L"Fight!");
        m_stateDuration = 1;
    }
}

void WidgetModalMessage::Update(f32 dt)
{
    if (m_viewMode == VIEW_CLOSED)
        return;

    m_time += dt;
    if (m_viewSubstate == Appearing)
        m_progress = Clamp(m_time / GUI_APPEAR_DURATION, 0.0f, 1.0f);
    else if (m_viewSubstate == Disappering)
        m_progress = Clamp(m_time / GUI_DISAPPEAR_DURATION, 0.0f, 1.0f);
    else if (m_viewSubstate == Static)
        m_progress = Clamp(m_time / m_stateDuration, 0.0f, 1.0f);

    if (m_progress == 1.0f) // next substate
    {
        m_time = 0.0f;
        m_viewSubstate = static_cast<ViewSubstate>(static_cast<int>(m_viewSubstate) + 1);
        if (m_viewSubstate > Disappering) // second loop
            SetState(VIEW_CLOSED);
    }
}

void WidgetModalMessage::Draw(r::Render& r, const mt::v2f& /*origin*/)
{
    if (m_viewMode == VIEW_FIGHT)
    {
        DrawHelper dr(r);
        dr.FillRect(Rectf(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT), 0xBB000000);
        dr.FillRect(Rectf(0, SCREEN_WIDTH, 200, SCREEN_HEIGHT - 200), 0xBB000000);
        if (m_viewSubstate == Appearing)
            m_textMain.Draw(r, v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + v2f(ErpDecelerate2(m_progress) * 100, 0), 4);
        else if (m_viewSubstate == Disappering)
            m_textMain.Draw(r, v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + v2f(100 + ErpDecelerate2(m_progress) * 700, 0), 4);
        else 
            m_textMain.Draw(r, v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + v2f(100, 0), 4);
    }
}