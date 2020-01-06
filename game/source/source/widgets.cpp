#include "widgets.h"
#include "res/sheet_mainmenu.h"

using namespace mt;
using namespace in;
using namespace r;


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

    m_sheetSprite->Draw(r, mainmenu::IconControllerKeyboardLeftPart, v2f(100,100), COLOR_WHITE);
    m_sheetSprite->Draw(r, mainmenu::LabelP1, v2f(180, 100), COLOR_WHITE);

    m_sheetSprite->Draw(r, mainmenu::IconControllerKeyboardRightPart, v2f(100, 200), COLOR_WHITE);
    m_sheetSprite->Draw(r, mainmenu::LabelP2, v2f(180, 200), COLOR_WHITE);
}
