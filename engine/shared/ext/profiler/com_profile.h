#ifndef COM_PROFILE_H
#define COM_PROFILE_H

#include "core/common/com_misc.h"
#include "ext/gui/gui_base.h"
#include "ext/strings/str_base.h"

namespace profiler
{

  //////////////////////////////////////////////////////////////////////////
  // profiler

  class ProfileNode
  {
    DENY_COPY(ProfileNode)
  public:
    ProfileNode(const char* const name, const wchar_t* const wname);
    u64                  m_time_inside;
    const char* const    m_name;
    const wchar_t* const m_wname;
    u32                  m_depth;
    u32                  m_count;
    const ProfileNode*   m_parent;
    ProfileNode*         m_next;

    static void ResetTimes();
    static ProfileNode* s_first;
    static u32          s_depth;
    static const ProfileNode* s_current;

    void Enter()
    {
      m_parent = s_current;
      s_current = this;
      m_depth  = s_depth;
      ++m_count;
      ++s_depth;
      m_time_inside -= GetCPUCycles();
    }
    void Leave()
    {
      m_time_inside += GetCPUCycles();
      --s_depth;
      s_current = m_parent;
    }
    void Reset()
    {
      m_time_inside = 0;
      m_count       = 0;
    }
  };

  class Marker
  {
    DENY_COPY(Marker)
  public:
    Marker(ProfileNode& node)
      : m_node(node)
    { m_node.Enter();}
    ~Marker()
    { m_node.Leave(); }

  private:
    ProfileNode& m_node;
  };


  //////////////////////////////////////////////////////////////////////////
  // gui

  class ProfileLayer;

  class ProfileLabel
    : public gui::TextLabel
  {
    DENY_COPY(ProfileLabel);
  public:
    enum Mode
    {
      plPerSample = 0,
      plPerCall   = 1,
      // $ todo:
      // plAverage
      plMax       = 2,
    };
    ProfileLabel(const r::BitmapFont* pfnt, const char* const szNodeName, const wchar_t* const szFriendlyName, Mode mode, ProfileLayer* owner = NULL)
      : m_node(NULL)
      , m_szNodeName(szNodeName)
      , m_szFriendlyName(szFriendlyName)
      , m_len(str::string_len(szFriendlyName))
      , m_mode(mode)
      , m_owner(owner)
      , gui::TextLabel(pfnt, szFriendlyName)// be sure szNodeName and szFriendlyName are kept alive
    {}
    virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
    {
      Update();
      gui::TextLabel::Draw(r, pivot, fOpacity);
    }
    virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/);

    Mode                 m_mode;
  private:
    void Update();
    void Lookup();
    const char*    const m_szNodeName;
    const wchar_t* const m_szFriendlyName;
    const u32            m_len;
    ProfileLayer*        m_owner;
    ProfileNode*         m_node;
  };

  class ProfileHistory
  {
  private:
    ProfileHistory& operator=(const ProfileHistory&);
  public:
    const ProfileNode* const m_source;
    const static u8 s_size = 64;

    ProfileHistory(const ProfileNode* source);

    u64 m_times[s_size];
    u8  m_cur;

#ifndef SMOOTH_PROFILER
    void Update()
    {
      m_times[m_cur] = m_source->m_time_inside;
      ++m_cur %= s_size;
    }
#else // SMOOTH_PROFILER
    const static u8 s_rate = SMOOTH_PROFILER;
    u64 m_time;
    u8  m_rateId;

    void Update()
    {
      m_time += m_source->m_time_inside;
      if(++m_rateId < s_rate)
        return;
      m_times[m_cur] = m_time / s_rate;
      ++m_cur %= s_size;
      m_rateId = 0;
      m_time   = 0;
    }
#endif // SMOOTH_PROFILER
  };

  class ProfileGraph
    : public gui::Widget
  {
  public:
    enum eDrawMode
    {
      dmAdd  = 0, // additive samples graph heights (drawn as bars)
      dmZero = 1, // all samples graph are zero aligned (drawn as lines or dots)
    };

    void AddNode(const ProfileNode*);
    void AddChildNodes(const ProfileNode* parent);
    void Clear();
    void Update();
    ProfileGraph();
    ~ProfileGraph();

    // IInputTarget
    virtual void Input(const mt::v2i16& /*p*/, in::InputEvent::eAction /*type*/, int /*id*/);

    // IVisual
    void Draw(r::Render&, const mt::v2i16& /*pivot*/, float /*fOpacity*/);
    virtual mt::v2u16 GetSize() const { return mt::v2u16(s_sample_w * ProfileHistory::s_size, (u16)m_height); }

  private:
    const static u8  s_max_logs = 16;
    const static u32 s_sample_w = 1;

    eDrawMode        m_drawMode;
    u8               m_nlogs;
    f32              m_height;
    f32              m_recip_norm;
    ProfileHistory*  m_logs[s_max_logs];
  };

  class ProfileLayer
    : public gui::Button::IListener
  {
  public:
    ProfileLayer();
    void Init(const r::BitmapFont* pfnt, const r::Sprite* p_sprbtn, const mt::v2f& pos);
    void Draw(r::Render& r);
    void SetParentNode(const ProfileNode*);
    void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id);
    // from gui::Button::IListener
    virtual void OnPressed(gui::Button* pBtn);
  private:
    void UpdateUI();
    const ProfileNode*     m_parent;
    bool                   m_changed;
    bool                   m_visible;
    bool                   m_graph;
    const r::Sprite*       m_sprbtn;
    const r::BitmapFont*   m_fnt;
    gui::ContainerAbsolute m_gui;
    ProfileLabel::Mode     m_mode;
    mt::v2f                m_pos;
    mt::v2f                m_back[4];
    u32                    m_bgcolor[4];

    enum Buttons
    {
      pbToggle = 0,
      pbUp     = 1,
      pbMode   = 2,
      pbGraph  = 3,
    };
  };

  //////////////////////////////////////////////////////////////////////////
  // StopWatch

  class StopWatch
  {
    DENY_COPY(StopWatch)
  public:
    StopWatch(const char* tag, const char* msg) // be sure that pointers are actual during StopWatch lifetime!
      : m_time(GetCPUCycles())
      , m_tag(tag)
      , m_msg(msg)
    {}
    ~StopWatch(){
      LogTime();
    }
    void LogTime(const char* additional_msg = NULL) const;
  private:
    const unsigned long long m_time;
    const char* m_tag;
    const char* m_msg;
  };

} // namespace profiler

#define _TEXT(t) L ## t

#ifdef PROFILER_ENABLED
#  define PROFILE_NODE(name) static profiler::ProfileNode _##name(#name, _TEXT(#name)); profiler::Marker m##name(_##name);
#  define PROFILE_RESET profiler::ProfileNode::ResetTimes();
#  define PROFILER_DECLARE         profiler::ProfileLayer m_profiler
#  define PROFILER_INIT(font, pcs, p) m_profiler.Init(font, pcs, p)
#  define PROFILER_INPUT(p, t, i)  m_profiler.Input(p, t, i)
#  define PROFILER_RENDER(r)       m_profiler.Draw(r)
#else // PROFILER_ENABLED
#  define PROFILE_NODE(name)
#  define PROFILE_RESET
#  define PROFILER_DECLARE
#  define PROFILER_INIT(font, pcs, p) {font; pcs; p;}
#  define PROFILER_INPUT(p, t, i)  {p; t; i;}
#  define PROFILER_RENDER(r)       {r;}
#endif // PROFILER_ENABLED

#endif // COM_PROFILE_H
