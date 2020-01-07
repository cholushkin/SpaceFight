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

    // draw player label
    m_res.m_sheet->Draw(r, GetPlayerLabelSprite(playerComp.m_playerID), pivot);
}

