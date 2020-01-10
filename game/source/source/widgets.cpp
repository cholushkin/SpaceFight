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
    m_time += dt;
    if (m_time > GUI_APPEAR_DURATION)
        return;
    m_appearProgress = Clamp(m_time / GUI_APPEAR_DURATION, 0.0f, 1.0f);
    m_appearOffset = erps::GetErp(ERP_BOUNCE)(m_appearProgress) * 100 - 100;
}

void WidgetPlayerDashboard::SetPlayer(int playerID)
{
    m_playerID = playerID;
}

void WidgetPlayerDashboard::Draw(r::Render& r, const mt::v2f& origin)
{
    DrawHelper dr(r);

    auto pivot = origin;
    pivot.x = origin.x + m_appearOffset;

    m_registry.view<PlayerComponent>().each(
        [&](auto /*ent*/, PlayerComponent& playerComp)
    {
        if (playerComp.m_playerID == m_playerID)
        {
            // draw background panel
            dr.FillRect(Rectf(pivot.x, pivot.x + 100, pivot.y, pivot.y + 40), COLOR_BEIGE);

            // draw energy
            auto energyValue = Clamp(playerComp.m_energy, 0.0f, 100.0f);
            auto length = energyValue / SHIP_ENERGY_MAX;
            m_text.SetText(m_res.m_fnt, str::StringBuilderW()(L"energy: %0", (int)energyValue));
            dr.FillRect(Rectf(pivot.x, pivot.x + 100 * length, pivot.y + 10, pivot.y + 20 + 10), COLOR_ORANGE);
            m_text.Draw(r, pivot + v2f(50, 10));

            // draw victories
            for (int i = 0; i < playerComp.m_winCount; ++i)
                m_res.m_sheet->Draw(r, gameplay::LabelVictory, pivot + v2f(34.0f + 22.0f*i, 0), COLOR_WHEAT);

            // draw player label
            m_res.m_sheet->Draw(r, playerComp.m_playerID == 0 ? gameplay::LabelP1 : gameplay::LabelP2, pivot);
            return;
        }
    });
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
    m_state = Closed;
}

void WidgetModalMessage::Show(const wchar_t* mainText, const wchar_t* secondaryText, float duration)
{
    m_state = Appearing;
    m_time = 0.0f;
    m_progress = 0.0f;

    m_textMain.SetText(m_res.m_fnt, mainText);
    m_textSecondary.SetText(m_res.m_fnt, secondaryText);
    m_stateDuration = duration;
}

void WidgetModalMessage::Close()
{
    m_state = Disappering;
    m_time = 0.0f;
    m_progress = 0.0f;
}

void WidgetModalMessage::Update(f32 dt)
{
    if (m_state == Closed)
        return;

    m_time += dt;
    auto duration = m_state == Appearing ?
        GUI_APPEAR_DURATION
        : m_state == Disappering ? GUI_DISAPPEAR_DURATION : m_stateDuration;

    m_progress = Clamp(m_time / duration, 0.0f, 1.0f);

    if (m_progress >= 1.0f) // next substate
    {
        m_time = 0.0f;
        if (m_state < Closed)
            m_state = static_cast<State>(m_state + 1);
    }
}

void WidgetModalMessage::Draw(r::Render& r, const mt::v2f& /*origin*/)
{
    if (m_state == Closed)
        return;

    DrawHelper dr(r);
    dr.FillRect(Rectf(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT), 0x11000000);
    dr.FillRect(Rectf(0, SCREEN_WIDTH, 200, SCREEN_HEIGHT - 200), 0xBB000000);

    auto offset = m_state == Appearing
        ? 100 - ErpDecelerate2(m_progress) * 100
        : m_state == Disappering ? ErpDecelerate2(m_progress) * 1000 : 0;

    m_textMain.Draw(r, v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + v2f(offset, 0), 3);
    m_textSecondary.Draw(r, v2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + v2f(offset, 64), 1);
}