#include "ext/profiler/com_profile.h"
#include "ext/strings/str_stringbuilder.h"
#include "core/log/log_log.h"

namespace profiler 
{
  using namespace str;
  
  ProfileNode* ProfileNode::s_first   = NULL;
  u32          ProfileNode::s_depth   = 0;
  const ProfileNode* ProfileNode::s_current = NULL;

  ProfileNode::ProfileNode(const char* const name, const wchar_t* const wname)
    : m_name(name)
    , m_wname(wname)
    , m_time_inside(0)
    , m_next(NULL)
    , m_depth(0)
    , m_parent(NULL)
  {
    m_next = s_first;
    s_first = this;
  }

  void ProfileNode::ResetTimes()
  {
    s_depth = 0;
    s_current = NULL;
    ProfileNode* pn = ProfileNode::s_first;
    while(pn)
    {
      pn->Reset();
      pn = pn->m_next;
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // ProfileLabel

  void ProfileLabel::Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/)
  {
    const mt::v2u16 size = GetSize();
    if(in::InputEvent::iaUp != type)
      return;
    if(p.x < 0 || p.x > size.x)
      return;
    if(p.y < 0 || p.y > size.y)
      return;
    if(m_owner)
    {
      m_owner->SetParentNode(m_node);
      return;
    }
    m_mode = (Mode)(1 + m_mode);
    if(m_mode == plMax)
      m_mode = (Mode)0;
  }

  void ProfileLabel::Update()
  {
    Lookup();
    if(!m_node)
      return;
    static wchar_t szText[255];
    
    // name
    memcpy(&szText[0], m_szFriendlyName, m_len * sizeof(szText[0]));
    u32 dis = m_len;
    szText[dis++] = L':';

    // mode
    if(plPerCall == m_mode)
      szText[dis++] = L'C';
    else if(plPerSample == m_mode)
      szText[dis++] = L'S';
        szText[dis++] = L':';

    // count
    dis += dectostring(&szText[dis], m_node->m_count);
    szText[dis++] = L':';
    u64 tpsm = GetCPUCyclesPerMSec();
    if(0 == tpsm)
      tpsm = 1;

    // time
    u64 time = m_node->m_time_inside;
    if(plPerCall == m_mode)
      time /= m_node->m_count;

    if(time < tpsm)
    {
      // we will get microseconds there
      dis += dectostring(&szText[dis], (u32) (time * 1000 / tpsm));
      szText[dis++] = L' ';
      szText[dis++] = L'u';
      szText[dis++] = L's';
      szText[dis]   = L'\0';
    }
    else
    {
      // we will get milliseconds there
      dis += dectostring(&szText[dis], (u32) (time / tpsm));
      szText[dis++] = L' ';
      szText[dis++] = L'm';
      szText[dis++] = L's';
      szText[dis]   = L'\0';
    }
    SetText(szText);
  }

  void ProfileLabel::Lookup()
  {
    if(m_node)
      return;
    ProfileNode* pn = ProfileNode::s_first;
    while(pn)
    {
      if(0 == stricmp(m_szNodeName, pn->m_name))
      {
        m_node = pn;
        break;
      }
      pn = pn->m_next;
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // ProfileHistory

  ProfileHistory::ProfileHistory(const ProfileNode* source)
    : m_source(source)
    , m_cur(0)
#ifdef SMOOTH_PROFILER
    , m_time(0)
    , m_rateId(0)
#endif
  {
    memset(&m_times[0], 0, sizeof(m_times[0]) * s_size);
  }

  //////////////////////////////////////////////////////////////////////////
  // ProfileGraph

  void ProfileGraph::AddNode(const ProfileNode* pn)
  {
    if(m_nlogs >= s_max_logs)
      return;
    m_logs[m_nlogs] = new ProfileHistory(pn);
    ++m_nlogs;
  }

  void ProfileGraph::AddChildNodes(const ProfileNode* parent)
  {
    const ProfileNode* pn = ProfileNode::s_first;
    while(pn)
    {
      if(pn->m_parent == parent)
        AddNode(pn);
      pn = pn->m_next;
    }
  }

  // Get color from id (6 good colors):
  // 0 FF0000 Red
  // 1 00FF00 Green
  // 2 FFFF00 Yellow
  // 3 0000FF Blue
  // 4 FF00FF Magenta
  // 5 00FFFF Aqua

  u32 GetColorByID(u32 id)
  {
    ++id;
    u32 color = mt::COLOR_BLACK;
    if(id & (1 << 0))
      color = SETR(color, 255);
    if(id & (1 << 1))
      color = SETG(color, 255);
    if(id & (1 << 2))
      color = SETB(color, 255);
    return color;
  }

  void ProfileGraph::Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/)
  {
    const mt::v2u16 size = GetSize();
    if(in::InputEvent::iaUp != type)
      return;
    if(p.x < 0 || p.x > size.x)
      return;
    if(p.y < 0 || p.y > size.y)
      return;
    m_drawMode = dmZero == m_drawMode ? dmAdd : dmZero;
  }

  void ProfileGraph::Draw(r::Render& r, const mt::v2i16& pivot, float /*fOpacity*/)
  {
    Update();
    r::Material m;
    const f32 sample_w = s_sample_w;
    {
      // draw back
      const mt::v2f back[] = 
      { 
        mt::v2f(pivot.x, pivot.y),
        mt::v2f(pivot.x + ProfileHistory::s_size * sample_w, pivot.y),
        mt::v2f(pivot.x + ProfileHistory::s_size * sample_w, pivot.y + m_height),
        mt::v2f(pivot.x, pivot.y + m_height)
      };
      const u32 color = SETA(mt::COLOR_WHITE, 128);
      const u32 colors[4] = {color, color, color, color};
      r.PostQuad(back, r::g_def_uv, colors, m);
    }

    if(dmZero == m_drawMode)
    {
      for (u8 id = 0; m_nlogs != id; ++id)
      {
        mt::v2f offset((f32)pivot.x, pivot.y + m_height);
        const ProfileHistory& ph = *m_logs[id];
        i8 sample = ph.m_cur;
        for (u8 s = 0; ProfileHistory::s_size != s; ++s)
        {
          i8 off = sample - s;
          if(off < 0)
            off += ProfileHistory::s_size;
          const u32 color = GetColorByID(id);
          const u32 colors[4] = {color, color, color, color};
          const f32 h = (f32)ph.m_times[off] * m_recip_norm;
          const mt::v2f scoords[] = 
          { 
            mt::v2f(offset.x,            offset.y - h),
            mt::v2f(offset.x + sample_w, offset.y - h),
            mt::v2f(offset.x + sample_w, offset.y - h + 1),
            mt::v2f(offset.x,            offset.y - h + 1)
          };
          r.PostQuad(scoords, r::g_def_uv, colors, m);
          offset.x += sample_w;
        }
      }
    }
    else
    {
      mt::v2f offset((f32)pivot.x, pivot.y + m_height);
      for (u8 s = 0; ProfileHistory::s_size != s; ++s)
      {
        for (u8 id = 0; m_nlogs != id; ++id)
        {
          const ProfileHistory& ph = *m_logs[id];
          i8 sample = ph.m_cur;
          i8 off = sample - s;
          if(off < 0)
            off += ProfileHistory::s_size;
          const u32 color = GetColorByID(id);

          const u32 colors[4] = {color, color, color, color};
          const f32 h = (f32)ph.m_times[off] * m_recip_norm;
          const mt::v2f scoords[] = 
          { 
            mt::v2f(offset.x,            offset.y - h),
            mt::v2f(offset.x + sample_w, offset.y - h),
            mt::v2f(offset.x + sample_w, offset.y),
            mt::v2f(offset.x,            offset.y)
          };
          r.PostQuad(scoords, r::g_def_uv, colors, m);
          offset.y -= h;
        }
        offset.x += sample_w;
        offset.y = pivot.y + m_height;
      }
    }
  }

  void ProfileGraph::Clear()
  {
    for (u8 id = 0; m_nlogs != id; ++id)
      delete m_logs[id];
    m_nlogs = 0;
  }

  void ProfileGraph::Update()
  {
    for (u8 id = 0; m_nlogs != id; ++id)
      m_logs[id]->Update();
  }

  ProfileGraph::ProfileGraph()
    : m_nlogs   (0)
    , m_height  (ProfileHistory::s_size * s_sample_w)
    , m_drawMode(dmZero)
    , m_recip_norm(0.0f)
  {
    const f32 den = (f32)(GetCPUCyclesPerMSec()) * (1000.0f / 60.0f);
    if(den > EPSILON)
      m_recip_norm = 1.0f / den * m_height;
  }

  ProfileGraph::~ProfileGraph()
  {
    Clear();
  }

  //////////////////////////////////////////////////////////////////////////
  // ProfileLayer

  ProfileLayer::ProfileLayer()
    : m_parent(NULL)
    , m_sprbtn(NULL)
    , m_fnt(NULL)
    , m_changed(false)
    , m_visible(false)
    , m_mode(ProfileLabel::plPerSample)
    , m_graph(false)
  {
    const u32 c = SETA(mt::COLOR_WHITE, 128);
    m_bgcolor[0] = c;
    m_bgcolor[1] = c;
    m_bgcolor[2] = c;
    m_bgcolor[3] = c;
  }

  void ProfileLayer::Draw(r::Render& r)
  {
    EASSERT(NULL == ProfileNode::s_current); // do not try to render under profile marker
    if(m_changed)
      UpdateUI();
    if(m_visible)
      r.PostQuad(m_back, r::g_def_uv, m_bgcolor, r::Material());
    m_gui.Draw(r, m_pos, 1.0f);
  }

  void ProfileLayer::Init(const r::BitmapFont* pfnt, const r::Sprite* p_sprbtn, const mt::v2f& pos)
  {
    m_pos = pos;
    m_fnt = pfnt;
    m_sprbtn = p_sprbtn;
    m_changed = true;
    UpdateUI();
  }

  void ProfileLayer::Input(const mt::v2i16& p, in::InputEvent::eAction type, int id)
  {
    m_gui.Input(p - m_pos, type, id);
  }

  void ProfileLayer::SetParentNode(const ProfileNode* pn)
  {
    m_parent = pn;
    m_changed = true;
  }

  // from gui::Button::IListener
  void ProfileLayer::OnPressed(gui::Button* pBtn)
  {
    if(pbToggle == pBtn->m_id)
    {
      m_visible = !m_visible;
      m_changed = true;
    }
    else if(pbUp == pBtn->m_id)
    {
      if(m_parent)
        SetParentNode(m_parent->m_parent);
    }
    else if(pbMode == pBtn->m_id)
    {
      m_mode = (ProfileLabel::Mode)(1 + m_mode);
      if(m_mode == ProfileLabel::plMax)
        m_mode = (ProfileLabel::Mode)0;
      m_changed = true;
    }
    else if(pbGraph == pBtn->m_id)
    {
      m_graph = !m_graph;
      m_changed = true;
    }
  }

  void ProfileLayer::UpdateUI()
  {
    if(!m_sprbtn || !m_fnt || !ProfileNode::s_first)
      return;
    m_changed = false;
    m_gui.Clear();
    mt::v2i16 pos(0, 0);
    mt::v2i16 ps = m_sprbtn->GetRect(0).RB();
    u16   fs = m_fnt->m_common.lineHeight;
    m_gui.Add(new gui::ButtonLabeled(pbToggle, m_sprbtn, this, m_fnt, m_visible ? L"H" : L"S"), pos);
    if(!m_visible)
      return;
    m_gui.Add(new gui::ButtonLabeled(pbUp,   m_sprbtn, this, m_fnt, L"U"), pos + mt::v2i16(1 * ps.x, 0));
    m_gui.Add(new gui::ButtonLabeled(pbMode, m_sprbtn, this, m_fnt, L"M"), pos + mt::v2i16(2 * ps.x, 0));
    m_gui.Add(new gui::ButtonLabeled(pbGraph,m_sprbtn, this, m_fnt, L"G"), pos + mt::v2i16(3 * ps.x, 0));
    
    pos.y = pos.y + ps.y;

    const ProfileNode* pn = ProfileNode::s_first;
    while(pn)
    {
      if(pn == m_parent)
      {
        m_gui.Add(new ProfileLabel(m_fnt, pn->m_name, pn->m_wname, m_mode, NULL), pos + mt::v2i16(8, 0));
        pos.y = pos.y + fs;
        break;
      }
      pn = pn->m_next;
    }

    pn = ProfileNode::s_first;
    while(pn)
    {
      if(pn->m_parent == m_parent)
      {
        m_gui.Add(new ProfileLabel(m_fnt, pn->m_name, pn->m_wname, m_mode, this), pos);
        pos.y = pos.y + fs;
      }
      pn = pn->m_next;
    }

    const mt::v2i16 size = m_gui.GetSize() + mt::v2u16(32, 0);// for digits
    m_back[0] = mt::v2f(0.0f,   0.0f)   + m_pos;
    m_back[1] = mt::v2f(size.x, 0.0f)   + m_pos;
    m_back[2] = mt::v2f(size.x, size.y) + m_pos;
    m_back[3] = mt::v2f(0.0f,   size.y) + m_pos;

    if(m_graph)
    {
      ProfileGraph* pg = new ProfileGraph();
      pg->AddChildNodes(m_parent);
      m_gui.Add(pg, mt::v2i16(size.x, 0));
    }
  }
  //////////////////////////////////////////////////////////////////////////
  // StopWatch

  void StopWatch::LogTime(const char* additional_msg) const
  {
    unsigned long long end_time = GetCPUCycles();
    unsigned long long tickspms = GetCPUCyclesPerMSec();
    unsigned long long delta    = end_time - m_time;

    str::StringBuilderF<char, 255> b;
    b(m_msg);
    if(additional_msg)
      b(additional_msg);
    if(delta < tickspms)
      b((u32) (delta * 1000 / tickspms))(" us\n"); // we will get microseconds there
    else
      b((u32) (delta        / tickspms))(" ms\n"); // we will get milliseconds there
    LOGI(m_tag, b.Get());
  }

} //namespace profiler

