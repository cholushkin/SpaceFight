#ifndef screenmainmenu_h__
#define screenmainmenu_h__

#include "application.h"
#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "ext/draw2d/draw_helper.h"

class ScreenMainMenu : public gui::BasicScreen
{
    DENY_COPY(ScreenMainMenu)
public:
    ScreenMainMenu(Application& g);

    virtual void Draw(r::Render& r);
    virtual void Update(f32 /*dt*/);
    virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id);

protected:
    const r::BitmapFont*    m_fnt;
    const r::SheetSprite*   m_sheetMainMenu;
    r::BitmapText           m_logoText;
    Application&            m_app;
};

#endif // screenmainmenu_h__
