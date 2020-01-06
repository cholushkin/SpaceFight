#ifndef widgets_h
#define widgets_h

#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"

class IWidget
{
    virtual void Update(f32 dt) = 0;
    virtual void Draw(r::Render& r, const mt::v2f& origin) = 0;
};

class WidgetControllerSelector: public IWidget
{
    DENY_COPY(WidgetControllerSelector)
public:
    WidgetControllerSelector(const r::SheetSprite* spr);
    void Update(f32 dt);
    void Draw(r::Render& r, const mt::v2f& origin);

private:
    const r::SheetSprite* m_sheetSprite;
    //GameContext* mGameContext;
    //u32 m_lifes;
    //u32 m_lifes_target;
    //ScreenGameplay& mScreen;
    //std::vector<f32> mScales;
};

#endif 