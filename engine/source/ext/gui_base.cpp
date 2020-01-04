#include "core/sound/snd_base.h"
#include "ext/gui/gui_base.h"
#include "ext/math/mt_colors.h"

NAMESPACE_BEGIN(gui)

using namespace mt;
using namespace in;
//////////////////////////////////////////////////////////////////////////
// ContainerAbsolute

Widget* ContainerAbsolute::Add(Widget* pw, const v2i16& pc)
{
  EASSERT(m_count < s_maxWidgets);
  if(s_maxWidgets == m_count)
    return pw;
  m_widgets[m_count] = pw;
  m_offsets[m_count] = pc;
  ++m_count;
  return pw;
}

void ContainerAbsolute::Move(const Widget * const pw, const v2i16& pc)
{
  for(u32 i = 0; m_count != i; ++i)
    if(pw == m_widgets[i]) {
      m_offsets[i] = pc;
      return;
    }

    EALWAYS_ASSERT("No such wiget");
}

// IInputTarget
void ContainerAbsolute::Input(const v2i16& p, InputEvent::eAction type, int id)
{
  for(u32 i = 0; m_count != i; ++i)
    if(m_widgets[i]->IsVisible())
      m_widgets[i]->Input(p - m_offsets[i], type, id);
}

// IVisual
void ContainerAbsolute::Draw(r::Render& r, const v2i16& pivot, float fOpacity)
{
  for(u32 i = 0; m_count != i; ++i)
    if(m_widgets[i]->IsVisible())
      m_widgets[i]->Draw(r, pivot + m_offsets[i], fOpacity);
}

mt::Recti16 ContainerAbsolute::GetRect() const
{
  if(0 == m_count)
    return mt::Recti16();

  mt::Recti16 res;
  for(u32 i = 0; m_count != i; ++i)
  {
    mt::Recti16 rect = m_widgets[i]->GetRect();
    rect.Offset(m_offsets[i]);
    if(0 != i)
      res.Cover(rect);
    else
      res = rect;
  }
  return res;
}

bool ContainerAbsolute::isIn(const mt::v2i16& p) const
{
  for(u32 i = 0; m_count != i; ++i)
  {
    const mt::Recti16 rect = m_widgets[i]->GetRect();
    if(rect.isIn(p - m_offsets[i]))
      return true;
  }
  return false;
}

Widget* ContainerAbsolute::GetControl(u32 idx)
{
  if(m_count > idx)
    return m_widgets[idx];
  EALWAYS_ASSERT("No such wiget");
  return NULL;
}

const Widget* ContainerAbsolute::GetControl(u32 idx) const
{
  if(m_count > idx)
    return m_widgets[idx];
  EALWAYS_ASSERT("No such wiget");
  return NULL;
}

const v2i16& ContainerAbsolute::GetPosition(u32 idx) const
{
  EASSERT(m_count > idx);
  return m_offsets[idx];
}

void ContainerAbsolute::SetPosition(u32 idx, const v2i16& pos)
{
  EASSERT(m_count > idx);
  m_offsets[idx] = pos;
}

void ContainerAbsolute::Clear()
{
  for(u32 i = 0; m_count != i; ++i)
    delete m_widgets[i];
  m_count = 0;
}

ContainerAbsolute::~ContainerAbsolute()
{
  Clear();
}

//////////////////////////////////////////////////////////////////////////
// Button

// IInputTarget
void Button::Input(const v2i16& p, InputEvent::eAction type, int id)
{
  if(-1 == m_touchID)
  {
    if(InputEvent::iaDown != type)
      return;
    if(!isIn(p))
      return;
    m_touchID = id;
    if(m_sndDown)
      m_sndDown->Play();
  }
  else
  {
    if(m_touchID != id)
      return;
    if(InputEvent::iaRepeat == type)
    {
      if(isIn(p))
        return;
      m_touchID = -1;
      return;
    }
    m_touchID = -1;
    if(InputEvent::iaUp != type)
      return;
    if(!isIn(p))
      return;
    if(m_listener)
      m_listener->OnPressed(this);
  }
}

// IVisual
void Button::Draw(r::Render& r, const v2i16& pivot, float fOpacity)
{
  if(NULL != m_sprite)
    m_sprite->Draw( r, -1 != m_touchID ? m_pressedFrame : m_normalFrame, v2f(pivot.x, pivot.y), make_color(fOpacity, 255, 255, 255));
}

mt::Recti16 Button::GetRect() const
{
  mt::Rectf r;
  if(m_sprite)
    r = m_sprite->GetRect(m_pressedFrame);
  return mt::Recti16(r.TL(), r.RB());
}

//////////////////////////////////////////////////////////////////////////
// ButtonLabeled

void ButtonLabeled::DrawLabel(r::Render& r, const mt::v2i16& pivot, float fOpacity)
{
  const mt::Recti16 bsize = Button::GetRect();
  const mt::v2u16   tsize = m_label.GetSize();
  r::BitmapText::eAlign a = m_label.GetAlign();

  i16 y_pos = (bsize.Height() - tsize.y) / 2 + bsize.top;

  if(r::BitmapText::tlCenter == a)
    m_label.Draw(r, pivot + mt::v2i16(bsize.Width() / 2 + m_margin + bsize.left, y_pos), fOpacity);
  else if(r::BitmapText::tlLeft == a)
    m_label.Draw(r, pivot + mt::v2i16(m_margin + bsize.left,                     y_pos), fOpacity);
  else if(r::BitmapText::tlRight == a)
    m_label.Draw(r, pivot + mt::v2i16(bsize.Width() - m_margin + bsize.left,    y_pos), fOpacity);
}

mt::Recti16 ButtonLabeled::GetRect() const
{
  if(m_sprite)
    return Button::GetRect();
  const mt::v2i16 size = m_label.GetSize();
  if(      r::BitmapText::tlCenter == m_label.GetAlign() )
    return mt::Recti16(-size.x / 2, size.x / 2, 0, size.y);
  else if( r::BitmapText::tlRight == m_label.GetAlign() )
    return mt::Recti16(-size.x, 0, 0, size.y);
  else
    return mt::Recti16(mt::v2i16(0,0), size);
}

//////////////////////////////////////////////////////////////////////////
// PushButton

// IVisual
void PushButton::Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
{
  if(NULL != m_sprite)
    m_sprite->Draw(r, m_normalFrame, pivot, make_color(fOpacity, 255, 255, 255),-1 != m_touchID ? m_sizeScale*0.9f:m_sizeScale);
}

mt::Recti16 PushButton::GetRect() const
{
  EASSERT(m_sprite);
  if(NULL == m_sprite)
    return  mt::Recti16(0, 0, 0, 0);
  return mt::Recti16(m_sprite->GetRect(m_normalFrame).Scale(m_sizeScale));
}

//////////////////////////////////////////////////////////////////////////
// CheckBox

// IInputTarget
void CheckBox::Input(const v2i16& p, InputEvent::eAction type, int /*id*/)
{
  Recti16 rect = GetRect();
  if(InputEvent::iaUp != type)
    return;
  if(!rect.isIn(p))
    return;
  m_checked = !m_checked;
  if(m_listener)
    m_listener->OnChanged(this);
  if(m_sndDown)
    m_sndDown->Play();
}

// IVisual
void CheckBox::Draw(r::Render& r, const v2i16& pivot, float fOpacity)
{
  if(!m_sprite)
    return;
  m_sprite->Draw(r, m_checked ? m_checkedFrame : m_normalFrame, v2f(pivot.x, pivot.y), make_color(fOpacity, 255, 255, 255));
}

//////////////////////////////////////////////////////////////////////////
// PushCheckBox

// IInputTarget
void PushCheckBox::Input(const mt::v2i16& p, in::InputEvent::eAction type, int id)
{
  if(-1 == m_touchID)
  {
    if(InputEvent::iaDown != type)
      return;
    if(!isIn(p))
      return;
    m_touchID = id;
  }
  else
  {
    if(m_touchID != id)
      return;
    if(InputEvent::iaRepeat == type)
    {
      if(isIn(p))
        return;
      m_touchID = -1;
      return;
    }
    m_touchID = -1;
    CheckBox::Input(p, type, id);
  }
}

// IVisual
void PushCheckBox::Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
{
  if(NULL != m_sprite)
    m_sprite->Draw(
    r,
    m_checked ? m_checkedFrame : m_normalFrame,
    mt::v2f(pivot.x, pivot.y),
    make_color(fOpacity, 255, 255, 255),
    -1 != m_touchID ? m_sizeScale * 0.9f : m_sizeScale);
}

//////////////////////////////////////////////////////////////////////////
// Screens

// $$$ Prototype
// Modal-style dialog with background
void Dialog::Draw(r::Render& r)
{
  // $$$ remove that
  m_bg.m_tl = m_position;
  m_bg.m_br = m_position + m_size;
  m_bg.m_opacity = m_opacity;

  m_bg.Draw(r);
  m_container.Draw(r, m_position, m_opacity);
}

void Dialog::CenterOnScreen(const v2i16& ss)
{
  m_position.x = (ss.x - m_size.x) / 2;
  m_position.y = (ss.y - m_size.y) / 2;
}

void Dialog::Close()
{
  if(m_owner)
    m_owner->Pop(this, true);
}

//////////////////////////////////////////////////////////////////////////
// ScreenManager
void ScreenManager::Push(Screen* ps)
{
  EASSERT(sMaxScreens != m_count);
  if(sMaxScreens == m_count)
    return;
  m_screens[m_count] = ps;
  ++m_count;
}

void ScreenManager::Pop(Screen* ps, bool bdestroy)
{
  EASSERT(m_count > 0);
  if(0 == m_count)
    return;
  --m_count;
  EASSERT(!ps || (ps == m_screens[m_count]));
  if(!bdestroy)
    return;
  m_destroy[m_dcount] = m_screens[m_count]; 
  ++m_dcount;
}

void ScreenManager::DestroyClosed()
{
  for(u16 s = 0 ; m_dcount != s; ++s)
    delete m_destroy[s];
  m_dcount = 0;
}

Screen* ScreenManager::GetTop() const
{
  return 0 == m_count ? NULL : m_screens[m_count - 1];
}

Screen* ScreenManager::GetFirst() const
{
  return 0 == m_count ? NULL : m_screens[0];
}

void ScreenManager::Input(const v2i16& p, InputEvent::eAction type, int id)
{
  Screen* screens[sMaxScreens];
  for(u16 s = 0 ; m_count != s; ++s)
    screens[s] = m_screens[s];
  for(u16 s = m_count ; 0 != s; --s)
  {
    screens[s - 1]->Input(p, type, id);
    if(screens[s - 1]->IsModal())
      break;
  }
  DestroyClosed();
}

void ScreenManager::Acceleration(const v3f& a)
{
  for(u16 s = m_count; 0 != s; --s)
  {
    m_screens[s - 1]->Acceleration(a);
    if(m_screens[s - 1]->IsModal())
      break;
  }
}

void ScreenManager::Draw(r::Render& r)
{
  for(u16 s = 0 ; m_count != s; ++s)
    m_screens[s]->Draw(r);
}

void ScreenManager::Update(f32 dt)
{
  const size_t count = m_count;
  Screen* screens[sMaxScreens];
  for(u16 s = 0 ; m_count != s; ++s)
    screens[s] = m_screens[s];
  for(u16 s = 0 ; count != s; ++s)
    screens[s]->Update(dt);
  DestroyClosed();
}

ScreenManager::ScreenManager()
  : m_count(0)
  , m_dcount(0)
{}

ScreenManager::~ScreenManager()
{
  for(u16 s = 0 ; m_count != s; ++s)
   delete m_screens[s];
  DestroyClosed();
}


//////////////////////////////////////////////////////////////////////////
// SidesLayout
SidesLayout::SidesLayout()
{
  for(size_t i = 0; sMax != i ; ++i)
    m_widgets[i] = NULL;
}

SidesLayout::~SidesLayout()
{
  for(size_t i = 0; sMax != i ; ++i)
    SAFE_DELETE(m_widgets[i]);
}

void SidesLayout::Set(Widget* w, SidesLayout::eSides where)
{
  if(m_widgets[where])
    SAFE_DELETE(m_widgets[where]);
  m_widgets[where] = w;
  if(m_widgets[where])
    Layout(where);
}

void SidesLayout::SetFrame(const v2i16& size)
{
  m_size = size;
  Layout();
}

void SidesLayout::Layout()
{
  for(size_t i = 0; sMax != i ; ++i)
    Layout((SidesLayout::eSides)i);
}

void SidesLayout::Layout(SidesLayout::eSides where)
{
  if(!m_widgets[where])
    return;
  const Recti16 r = m_widgets[where]->GetRect();
  if(sTopLeft == where)
    m_pos[where] = -r.TL();
  else if(sTopRight == where)
    m_pos[where] = v2i16(m_size.x - r.Width() - r.left, -r.top);
  else if(sBottomLeft == where)
    m_pos[where] = v2i16(-r.left, m_size.y - r.Height() - r.top);
  else if(sBottomRight == where)
    m_pos[where] = m_size - r.RB();
  else if(sCenter == where)
    m_pos[where] = v2i16((m_size.x - r.Width())/2 - r.left, (m_size.y - r.Height())/2 - r.top);
  else if(sTop == where)
    m_pos[where] = v2i16((m_size.x - r.Width())/2 - r.left, -r.top);
  else if(sBottom == where)
    m_pos[where] = v2i16((m_size.x - r.Width())/2 - r.left, m_size.y - r.Height() - r.top);
  else if(sLeft == where)
    m_pos[where] = v2i16(-r.left, (m_size.y - r.Height())/2 - r.top);
  else if(sRight == where)
    m_pos[where] = v2i16(m_size.x - r.right, (m_size.y - r.Height())/2 - r.top);
}

// from IInputTarget
void SidesLayout::Input(const v2i16& p, in::InputEvent::eAction type, int id)
{
  for(size_t i = 0; sMax != i ; ++i)
    if(m_widgets[i] && m_widgets[i]->IsVisible())
      m_widgets[i]->Input(p - m_pos[i], type, id);
}

void SidesLayout::Acceleration(const v3f& a)
{
  for(size_t i = 0; sMax != i ; ++i)
    if(m_widgets[i] && m_widgets[i]->IsVisible())
      m_widgets[i]->Acceleration(a);
}

// from IVisual
void SidesLayout::Draw(r::Render& r, const v2i16& pivot, float fOpacity)
{
  for(size_t i = 0; sMax != i ; ++i)
    if(m_widgets[i] && m_widgets[i]->IsVisible())
      m_widgets[i]->Draw(r, pivot + m_pos[i], fOpacity);
}

bool SidesLayout::isIn(const mt::v2i16& p) const
{
  for(size_t i = 0; sMax != i ; ++i)
    if(m_widgets[i] && m_widgets[i]->isIn(p - m_pos[i]))
      return true;
  return false;
}

//////////////////////////////////////////////////////////////////////////
// LinearLayout

LinearLayout::LinearLayout(eDirection d, eAlign a)
  : m_direction(d)
  , m_align(a) {}

Widget* LinearLayout::Add(Widget* w, i16 gap)
{
  EASSERT(m_count < s_maxWidgets);
  if(s_maxWidgets == m_count)
    return w;
  int axis0 = dVertical == m_direction ? 1 : 0;

  if(0 == m_count)
    gap = 0;

  m_offsets[m_count][axis0] = GetSize()[axis0] + gap - (1 == axis0 ? w->GetRect().top : w->GetRect().left);
  m_widgets[m_count] = w;
  ++m_count;

  Layout();

  return w;
}

void LinearLayout::Layout()
{
  int axis1 = dVertical == m_direction ? 0 : 1;
  u16 max_size = 0;

  if(aStart != m_align)
    for(size_t i = 0; m_count != i ; ++i)
      max_size = mt::Max(max_size, m_widgets[i]->GetSize()[axis1]);

  if(aStart == m_align)
    for(size_t i = 0; m_count != i ; ++i)
      m_offsets[i][axis1] = -(1 == axis1 ? m_widgets[i]->GetRect().top : m_widgets[i]->GetRect().left);
  else if(aCenter == m_align)
    for(size_t i = 0; m_count != i ; ++i)
      m_offsets[i][axis1] = (max_size - m_widgets[i]->GetSize()[axis1])/2 - (1 == axis1 ? m_widgets[i]->GetRect().top : m_widgets[i]->GetRect().left);
  else if(aEnd == m_align)
    for(size_t i = 0; m_count != i ; ++i)
      m_offsets[i][axis1] =  max_size - m_widgets[i]->GetSize()[axis1]    - (1 == axis1 ? m_widgets[i]->GetRect().top : m_widgets[i]->GetRect().left);
}

NAMESPACE_END(gui)
