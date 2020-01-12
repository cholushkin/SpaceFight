#include "application.h"
#include "res/resources.h"
#include "screenmainmenu.h"
#include "config.h"
#include "core/log/log_log.h"
#include "ext/log/log_loggers.h"

using namespace in;
using namespace res;
using namespace mt;
using namespace r;

Application::Application() : m_rpool(64)
{
}

bool Application::Init()
{
    // console 
#if defined(_DEBUG) && defined(SHOW_CONSOLE)
    dlog::Log::GetInstance()->EnableLogger(0, new dlog::LoggerSysConsole("console", 120, 1200));
    dlog::Log::GetInstance()->GetLogger(0)->SetLevelFilter(dlog::Log::PRIORITY_DEBUG);
    NLOGI_COLOR("sf", "Space fight v.0.1. Initializing[c:ff0000].[c:00ff00].[c:0000ff].[c:ffffff]");
#endif

    // renderer
    if (!m_render.Init())
        return false;

    Device::sContext.SetUserScreenSize(
        mt::v2f(SCREEN_WIDTH, SCREEN_HEIGHT),
        Context::sfFitFullScreen);

    // basic
    m_time = GetCPUCycles();
    m_dtime = 0;
    m_screenmgr.Push(new ScreenMainMenu(*this));

    NLOGI("sf", "done.");
    return true;
}

void Application::Release()
{
    m_screenmgr.Pop();
    m_screenmgr.DestroyClosed();
    m_rpool.Clear();
    m_render.Release();
}

void Application::RenderFrame()
{
    m_render.Clear();
    m_screenmgr.Draw(m_render);
    m_render.Present();
}

void Application::Tick()
{
    const unsigned long long cps = GetCPUCyclesPerMSec();
    const unsigned long long new_time = GetCPUCycles();
    m_dtime = new_time - m_time;
    m_dtime = Clamp<unsigned long long>(m_dtime, 1000 * cps / 65, 1000 * cps / 60);
    m_time = new_time;
    m_screenmgr.Update(m_dtime / 1000.0f / cps);
}

void Application::Input(const v2i16& pos, InputEvent::eAction type, unsigned int id)
{
    m_screenmgr.Input(pos, type, id);
    m_touches.Input(pos, type, id);
}

void Application::Acceleration(const v3f& /*a*/)
{
}

Application g_app;

// callbacks

// --- touch inputs
extern "C" void input_touch_begin(int x, int y, unsigned int id)
{
    Device::sContext.MapInput(x, y);
    g_app.Input(v2i16((i16)x, (i16)y), InputEvent::iaDown, id);
}

extern "C" void input_touch_move(int x, int y, unsigned int id)
{
    Device::sContext.MapInput(x, y);
    g_app.Input(v2i16((i16)x, (i16)y), InputEvent::iaRepeat, id);
}

extern "C" void input_touch_end(int x, int y, unsigned int id)
{
    Device::sContext.MapInput(x, y);
    g_app.Input(v2i16((i16)x, (i16)y), InputEvent::iaUp, id);
}

extern "C" void input_touch_cancel(int x, int y, unsigned int id)
{
    Device::sContext.MapInput(x, y);
    g_app.Input(v2i16((i16)x, (i16)y), InputEvent::iaCancel, id);
}

// -----
extern "C" void screen_resize(unsigned int w, unsigned int h)
{
    Device::sContext.SetPhysScreenSize(v2i16((i16)w, (i16)h));
    Device::sContext.SetUserScreenSize(Device::sContext.GetPhysScreenSize(), Context::sfFitFullScreen);
}

extern "C" void input_acceleration(float x, float y, float z)
{
    g_app.Acceleration(v3f(x, y, z));
}

extern "C" bool game_init()
{
    return g_app.Init();
}

extern  "C" void game_tick()
{
    g_app.Tick();
}

extern  "C" void game_render()
{
    g_app.RenderFrame();
}

extern  "C" void game_release()
{
    g_app.Release();
}