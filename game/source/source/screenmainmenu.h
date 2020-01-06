#ifndef screenmainmenu_h__
#define screenmainmenu_h__

#include "application.h"
#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "ext/draw2d/draw_helper.h"
#include "widgets.h"

class ScreenMainMenu : public gui::BasicScreen
{
    DENY_COPY(ScreenMainMenu)
public:
    ScreenMainMenu(Application& g);
    ~ScreenMainMenu();

    virtual void Draw(r::Render& r);
    virtual void Update(f32 /*dt*/);

protected:
    const r::BitmapFont*        m_fnt;
    r::BitmapText               m_logoText;
    r::BitmapText               m_promtText;
    const r::SheetSprite*       m_sheetMainMenu;
    WidgetControllerSelector    m_controllerSelector;
    Application&                m_app;
};

#endif // screenmainmenu_h__
