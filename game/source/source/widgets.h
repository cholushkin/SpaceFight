#ifndef widgets_h
#define widgets_h

#include "ext/gui/gui_base.h"
#include "ext/primitives/r_primitives.h"
#include "PlayerComponent.h"
#include "entt/entt.hpp"


class IWidget
{
    virtual void Update(f32 dt) = 0;
    virtual void Draw(r::Render& r, const mt::v2f& origin) = 0;
};


struct GameResources;
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

class WidgetPlayerDashboard: public IWidget
{
    DENY_COPY(WidgetPlayerDashboard)
public:
    WidgetPlayerDashboard(entt::DefaultRegistry& registry, const GameResources& res);
    void Update(f32 dt);
    void Draw(r::Render& r, const mt::v2f& origin);
    void SetPlayer(const uint32_t& ent);
private:
    const GameResources&        m_res;
    entt::DefaultRegistry&      m_registry;
    r::BitmapText               m_text;
    float                       m_time;
    float                       m_appearOffset;
    float                       m_appearProgress;
    uint32_t                    m_playerID;
};

#endif 