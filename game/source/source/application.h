#ifndef application_h
#define application_h

#include "core/render/r_render.h"
#include "core/sound/snd_base.h"
#include "ext/primitives/r_font.h"
#include "ext/primitives/r_primitives.h"
#include "ext/gui/gui_base.h"

class Application
{
    DENY_COPY(Application);
public:
    bool Init();
    void Release();
    void RenderFrame();
    void Tick();

    void Input(const mt::v2i16& p, in::InputEvent::eAction type, unsigned int id);
    void Acceleration(const mt::v3f& a);
    Application();

    r::Render&          GetRender() { return m_render; }
    res::ResourcesPool& GetRPool() { return m_rpool; }
    snd::SoundManager&  GetSMGR() { return m_sndmgr; }
    gui::ScreenManager& GetScreenManager() { return m_screenmgr; }

private:
    r::Render             m_render;
    res::ResourcesPool    m_rpool;
    snd::SoundManager     m_sndmgr;
    gui::ScreenManager    m_screenmgr;
    unsigned long long    m_time;
    unsigned long long    m_dtime;
    in::TouchTracker      m_touches;
};


#endif // application_h
