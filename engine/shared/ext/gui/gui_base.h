#ifndef GUI_BASE_H
#define GUI_BASE_H

#include "ext/primitives/r_primitives.h"
#include "ext/primitives/r_font.h"
#include "ext/primitives/r_sheet_sprite.h"
#include "ext/input/in_touchtracker.h"

namespace snd
{
  class Sound;
}

// [] implement GetSize() via GetRect()

//////////////////////////////////////////////////////////////////////////
// Interfaces


NAMESPACE_BEGIN(gui)

class IInputTarget
{
public:
  virtual void Input(const mt::v2i16& /*p*/, in::InputEvent::eAction /*type*/, int /*id*/){}
  virtual void Acceleration(const mt::v3f& /*a*/){}
  virtual ~IInputTarget(){} // for safety
};

class IVisual
{
public:
  virtual void Draw(r::Render&, const mt::v2i16& /*pivot*/, float /*fOpacity*/){}
  virtual mt::v2u16   GetSize() const { return GetRect().Size(); }
  virtual mt::Recti16 GetRect() const { return mt::Recti16(0, 0, 0, 0); }
  virtual bool        isIn(const mt::v2i16& p) const { return GetRect().isIn(p); }
  virtual ~IVisual(){} // for safety
};

class Widget
  : public IVisual
  , public IInputTarget
{
public:
  Widget() : m_bIsVisible(true){}
  void SetVisible(bool v){
    m_bIsVisible = v;
  }
  bool IsVisible() const{
    return m_bIsVisible;
  }
protected:
  bool m_bIsVisible;
};

class ContainerAbsolute
  : public Widget
{
public:
  template<class W>
  W* Add(W* pw, const mt::v2i16& pc) {
    Add((Widget*)pw, pc);
    return pw;
  }
  Widget* Add(Widget* pw, const mt::v2i16& pc);
  void Move(const Widget* const pw, const mt::v2i16& pc);

  // IInputTarget
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id);

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);
  virtual mt::Recti16 GetRect() const;
  virtual bool isIn(const mt::v2i16& p) const;  

  u32 GetNControls() const { return m_count; }

  Widget*          GetControl(u32 idx);
  const Widget*    GetControl(u32 idx) const;

  const mt::v2i16& GetPosition(u32 idx) const;
  void             SetPosition(u32 idx, const mt::v2i16& pos);

  void Clear();

  ContainerAbsolute()
    : m_count(0){}

  virtual ~ContainerAbsolute();

protected: // make me private

  static const u32 s_maxWidgets = 32; // make me template
  Widget* m_widgets[s_maxWidgets];
  mt::v2i16  m_offsets[s_maxWidgets];
  u32    m_count;
};

// todo: remove Sprite constructors
class Button
  : public Widget
{
public:
  class IListener
  {
  public: 
    virtual void OnPressed(Button* pBtn) = 0;
  protected:
    ~IListener(){}
  };

  Button(u32 id, const r::Sprite* ps, IListener* pl, snd::Sound* pSndDown = NULL)
    : m_id(id), m_sprite(ps), m_listener(pl), m_sndDown(pSndDown)
    , m_pressedFrame(1), m_normalFrame(0), m_touchID(-1) {}

  Button(u32 id, const r::Sprite* ps, u16 normalFrame, u16 pressedFrame, IListener* pl, snd::Sound* pSndDown = NULL)
    : m_id(id), m_sprite(ps), m_listener(pl), m_sndDown(pSndDown)
    , m_pressedFrame(pressedFrame), m_normalFrame(normalFrame), m_touchID(-1) {}

  // IInputTarget
  virtual void  Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/);
  
  // IVisual
  virtual void  Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);

  virtual mt::Recti16 GetRect() const;

  const u32  m_id;

protected:

  u16 m_pressedFrame;
  u16 m_normalFrame;

  const r::Sprite* m_sprite; // 2 frames: normal/pressed state
  int        m_touchID; // -1 is not pressed
  IListener* m_listener;

  snd::Sound* m_sndDown;

  Button(const Button&);
  Button& operator=(const Button&);
};



class TextLabel
  : public Widget
{
public:
  TextLabel(){}

  TextLabel(const r::BitmapFont* pFont, const wchar_t* str, int wrapWidth = -1)
  {
    m_text.SetText(pFont, str, wrapWidth);
  }

  TextLabel(const r::BitmapFont* pFont, const wchar_t* str, u32 color, int wrapWidth = -1)
  {
    m_text.SetColor(color);
    m_text.SetText(pFont, str, wrapWidth);
  }

  TextLabel(const r::BitmapFont* pFont, const wchar_t* str, u32 color, r::BitmapText::eAlign a, int wrapWidth = -1)
  {
    m_text.SetColor(color);
    m_text.SetText(pFont, str, wrapWidth);
    m_text.SetAlign(a);
  }

  void SetText(const wchar_t* str, int wrapWidth = -1)
  {
    m_text.SetText(str, wrapWidth);
  }

  void SetText(const r::BitmapFont* pFont, const wchar_t* str, int wrapWidth = -1)
  {
    m_text.SetText(pFont, str, wrapWidth);
  }

  void SetAlign(r::BitmapText::eAlign a)
  {
    m_text.SetAlign(a);
  }

  r::BitmapText::eAlign GetAlign() const
  {
    return m_text.GetAlign();
  }

  void SetColor(u32 c)
  {
    m_text.SetColor(c);
  }

  void SetFontScale(f32 s) // better be called before SetText
  {
    m_text.SetFontScale(s);
  }
  
  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
  {
    if(pivot != m_text.GetPosition())
      m_text.SetPosition(pivot);
    const u32 c = SETA(m_text.GetColor(), u8(fOpacity * 255));
    if(c != m_text.GetColor())
      m_text.SetColor(c);
    m_text.Draw(r);
  }

  virtual mt::Recti16 GetRect() const
  {
    const mt::v2i16 size = m_text.GetSize();
    if(      r::BitmapText::tlCenter == m_text.GetAlign() )
      return mt::Recti16(-size.x / 2, size.x / 2, 0, size.y);
    else if( r::BitmapText::tlRight == m_text.GetAlign() )
      return mt::Recti16(-size.x, 0, 0, size.y);
    else
      return mt::Recti16(mt::v2i16(0,0), size);
  }

protected:
  r::BitmapText m_text;

  TextLabel(const TextLabel&);
  TextLabel& operator=(const TextLabel&);
};

class ButtonLabeled
  : public Button
{
public:
  ButtonLabeled(
    u32 id, const r::Sprite* ps, IListener* pl,
    const r::BitmapFont* font, const wchar_t* label,
    snd::Sound* pSndDown = NULL,
    r::BitmapText::eAlign a = r::BitmapText::tlCenter, i16 margin = 0)
    : Button(id, ps, pl, pSndDown)
    , m_label(font, label)
    , m_margin(margin)
  {
    m_label.SetAlign(a);
  }

  ButtonLabeled(
    u32 id, const r::Sprite* ps, u16 normalFrame, u16 pressedFrame, IListener* pl,
    const r::BitmapFont* font, const wchar_t* label,
    snd::Sound* pSndDown = NULL,
    r::BitmapText::eAlign a = r::BitmapText::tlCenter, i16 margin = 0)
    : Button(id, ps, normalFrame, pressedFrame, pl, pSndDown)
    , m_label(font, label)
    , m_margin(margin)
  {
    m_label.SetAlign(a);
  }

  ButtonLabeled& SetColor(u32 c)
  {
    m_label.SetColor(c);
    return *this;
  }

  ButtonLabeled& SetAlign(r::BitmapText::eAlign a, i16 margin)
  {
    m_label.SetAlign(a);
    m_margin = margin;
    return *this;
  }

  ButtonLabeled& SetFontScale(f32 s)
  {
    m_label.SetFontScale(s);
    return *this;
  }

  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
  {
    Button::Draw(r, pivot, fOpacity);
    DrawLabel(r, pivot, fOpacity);
  }

  virtual mt::Recti16 GetRect() const;

  ButtonLabeled& SetText(const wchar_t* str, int wrapWidth = -1)
  {
    m_label.SetText(str, wrapWidth);
    return *this;
  }
protected:
  void DrawLabel(r::Render& r, const mt::v2i16& pivot, float fOpacity);
private:
  TextLabel m_label;
  i16       m_margin;

  ButtonLabeled(const TextLabel&);
  ButtonLabeled& operator=(const TextLabel&);
};

//////////////////////////////////////////////////////////////////////////
// PushButton
// working with sheet, pressed state from one sprite
class PushButton  : public Button
{
  DENY_COPY(PushButton);
public:
  PushButton(u32 id, u16 frame, const r::SheetSprite* ps, f32 size, Button::IListener* pl, snd::Sound* pSndDown = NULL)
    : gui::Button(id, ps, frame, frame, pl, pSndDown)
    , m_sizeScale(size)
  {}

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);
  virtual mt::Recti16 GetRect() const;

protected:
  f32                         m_sizeScale;
};

//////////////////////////////////////////////////////////////////////////
// ImageT

template <class SpriteT>
class ImageT
  : public Widget
{
public:
  ImageT(const SpriteT* ps)
    : m_sprite(ps), m_frame(0),     m_color(mt::COLOR_WHITE), m_scale(1.0f, 1.0f){}

  ImageT(const SpriteT* ps, u32 frame)
    : m_sprite(ps), m_frame(frame), m_color(mt::COLOR_WHITE), m_scale(1.0f, 1.0f){}

  ImageT(const SpriteT* ps, u32 frame, u32 color)
    : m_sprite(ps), m_frame(frame), m_color(color), m_scale(1.0f, 1.0f){}

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
  {
    if(NULL != m_sprite)
      m_sprite->Draw(r, m_frame, pivot, SETA(m_color, u8(fOpacity * 255)), m_scale);
  }
  
  virtual mt::Recti16 GetRect() const;

  void SetFrame(u32 f)
  {
    // m_sprite has EASSERT on that
    m_frame = f;
  }

  void SetScale(const mt::v2f& s)
  {
    m_scale = s;
  }

  u32 GetFrame() const
  {
    return m_frame;
  }

  u32 GetNFrames() const
  {
    EASSERT(m_sprite);
    return (NULL != m_sprite) ? m_sprite->GetNFrames() : 0;
  }
  
  const SpriteT* GetSprite() const
  {
    return m_sprite;
  }
protected:
  const SpriteT* m_sprite;
  u32     m_frame;
  u32     m_color;
  mt::v2f m_scale;

  ImageT(const ImageT&);
  ImageT& operator=(const ImageT&);
};

typedef ImageT<r::Sprite> Image;

template<>
inline mt::Recti16 Image::GetRect() const
{
  mt::Rectf r;
  if(m_sprite)
    r = m_sprite->GetRect(m_frame).Scale(m_scale);
  return mt::Recti16(r.TL(), r.RB());
}

class ColorRect
  : public Widget
{
public:
  ColorRect(mt::v2i16 tl, mt::v2i16 br, u32 color)
    : m_tl(tl), m_br(br), m_color(color){}

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float /*fOpacity*/)
  {
    const mt::v2f vs[4] = 
    {
      mt::v2f( (f32)pivot.x + m_tl.x, (f32)pivot.y + m_tl.y),
      mt::v2f( (f32)pivot.x + m_br.x, (f32)pivot.y + m_tl.y),
      mt::v2f( (f32)pivot.x + m_br.x, (f32)pivot.y + m_br.y),
      mt::v2f( (f32)pivot.x + m_tl.x, (f32)pivot.y + m_br.y)
    };
    const u32 colors[4] = {m_color, m_color, m_color, m_color};
    r.PostQuad(vs, r::g_def_uv, colors, r::Material(NULL, r::Material::bmAlpha));
  }

  virtual mt::Recti16 GetRect() const
  {
    return mt::Recti16(m_tl, m_br);
  }

private:
  mt::v2i16 m_tl;
  mt::v2i16 m_br;
  u32       m_color;
};

class CheckBox
  : public Widget
{
  DENY_COPY(CheckBox)
public:
  class IListener
  {
  public: 
    virtual void OnChanged(CheckBox* pCB) = 0;
  protected:
    ~IListener(){}
  };

  CheckBox(u32 id, const r::Sprite* ps, IListener* pl, bool bState, snd::Sound* pSndDown = NULL)
    : m_id(id), m_sprite(ps), m_listener(pl), m_checked(bState), m_sndDown(pSndDown), m_normalFrame(0), m_checkedFrame(0) {}

  CheckBox(u32 id, const r::Sprite* ps, u16 normalFrame, u16 checkedFrame, IListener* pl, bool bState, snd::Sound* pSndDown = NULL)
    : m_id(id), m_sprite(ps), m_listener(pl), m_checked(bState), m_sndDown(pSndDown), m_normalFrame(normalFrame), m_checkedFrame(checkedFrame) {}

  // IInputTarget
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/);

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);
  
  virtual mt::Recti16 GetRect() const
  {
    mt::Rectf r;
    if(m_sprite)
      r = m_sprite->GetRect(m_normalFrame);
    return mt::Recti16(r.TL(), r.RB());
  }

  bool isChecked() const
  { return m_checked; }
  
  void SetChecked(bool b) // no call to IListener!
  { m_checked = b; }

  const u32  m_id;
protected:
  const r::Sprite* m_sprite; // 2 frames: normal/pressed state
  bool             m_checked;
  IListener*       m_listener;
  snd::Sound*      m_sndDown;
  u16              m_normalFrame;
  u16              m_checkedFrame;
};


class CheckBoxLabeled
  : public CheckBox
{
public:
  /// $$$ Make 'shifted label' too
  CheckBoxLabeled(u32 id, const r::Sprite* ps, IListener* pl, const r::BitmapFont* font, const wchar_t* label, bool bState, snd::Sound* pSndDown = NULL)
    : CheckBox(id, ps, pl, bState, pSndDown)
    , m_label(font, label)
  {
    m_label.SetAlign(r::BitmapText::tlCenter);
  }

  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity)
  {
    CheckBox::Draw(r, pivot, fOpacity);
    const mt::v2u16 bsize = CheckBox::GetSize();
    const mt::v2u16 tsize = m_label.GetSize();
    m_label.Draw(r, pivot + mt::v2i16(bsize.x / 2, (bsize.y - tsize.y) / 2), fOpacity);
  }
  void SetColor(u32 c)
  {
    m_label.SetColor(c);
  }
private:
  TextLabel m_label;
};


//////////////////////////////////////////////////////////////////////////
// PushButton

class PushCheckBox : public CheckBox
{
  DENY_COPY(PushCheckBox);
public:
  PushCheckBox(u32 id, u16 frame_a, u16 frame_b, const r::SheetSprite* ps, f32 size, CheckBox::IListener* pl, bool isOn, snd::Sound* pSndDown = NULL)
    : gui::CheckBox(id, ps, frame_a, frame_b, pl, isOn, pSndDown)
    , m_sizeScale(size)
    , m_touchID(-1)
  {}
  // IInputTarget
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id);
  // IVisual
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);

protected:
  int   m_touchID; // -1 is not pressed
  f32   m_sizeScale;
};

//////////////////////////////////////////////////////////////////////////
// RadioGroup

template<unsigned int count>
class RadioGroup
  : public CheckBox::IListener
{
  static const unsigned int max_size = count;
public:
  RadioGroup(){}

  RadioGroup(CheckBox* cb0, CheckBox* cb1, int id = -1)
  {
    Init(cb0, cb1, id);
  }

  RadioGroup(CheckBox* cb0, CheckBox* cb1, CheckBox* cb2, int id = -1)
  {
    Init(cb0, cb1, cb2, id);
  }

  void Init(CheckBox* cb0, CheckBox* cb1, int id = -1)
  {
    int test[max_size == 2]; test;
    m_cbs[0] = cb0;
    m_cbs[1] = cb1;
    SetChecked(id);
  }

  void Init(CheckBox* cb0, CheckBox* cb1, CheckBox* cb2, int id = -1)
  {
    int test[max_size == 3]; test;
    m_cbs[0] = cb0;
    m_cbs[1] = cb1;
    m_cbs[2] = cb2;
    SetChecked(id);
  }

  void Init(CheckBox* cb0, CheckBox* cb1, CheckBox* cb2, CheckBox* cb3, int id = -1)
  {
    int test[max_size == 4]; test;
    m_cbs[0] = cb0;
    m_cbs[1] = cb1;
    m_cbs[2] = cb2;
    m_cbs[3] = cb3;
    SetChecked(id);
  }

  void Init(CheckBox* cb0, CheckBox* cb1, CheckBox* cb2, CheckBox* cb3, CheckBox* cb4, int id = -1)
  {
    int test[max_size == 5]; test;
    m_cbs[0] = cb0;
    m_cbs[1] = cb1;
    m_cbs[2] = cb2;
    m_cbs[3] = cb3;
    m_cbs[4] = cb4;
    SetChecked(id);
  }

  void SetChecked(int id)
  {
    EASSERT((int)max_size > id);
    if(id < 0)
      OnChanged(NULL);
    else
    {
      m_cbs[id]->SetChecked(true);
      OnChanged(m_cbs[id]);
    }
  }

  int GetChecked() const
  {
    for(size_t i = 0; max_size != i; ++i)
      if(m_cbs[i]->isChecked())
        return (int)i;
    return -1;
  }

  virtual void OnChanged(CheckBox* pCB)
  {
    if(pCB && !pCB->isChecked())
      pCB->SetChecked(true);
    else
      for(size_t i = 0; max_size != i; ++i)
        m_cbs[i]->SetChecked(m_cbs[i] == pCB);
  }
private:
  CheckBox* m_cbs[count];
};

//////////////////////////////////////////////////////////////////////////
// Screens

// all screens live in global coordinates and no hierarchy
class Screen
  : public IInputTarget
{
public:
  virtual void Draw(r::Render&){}
  virtual void Update(f32 /*dt*/){}
  virtual bool IsModal() const { return false; } // do not propagate input beyond that screen
  //?BlockInput
  //?TestHit
  virtual ~Screen(){}
};

// $$$ Prototype
class BasicScreen
  : public Screen
{
public:
  BasicScreen()
    : m_opacity(1.0f){}
  virtual void Draw(r::Render& r)
  {
    m_container.Draw(r, mt::v2i16(0, 0), m_opacity);
  }
  // IInputTarget
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id)
  {
    m_container.Input(p, type, id);
  }
protected:
  f32               m_opacity;
  ContainerAbsolute m_container;
};


class ScreenManager;
// $$$ Prototype
// Modal-style dialog with background
class Dialog
  : public Screen
{
public:
  Dialog(ScreenManager* owner)
    : m_opacity(1.0f), m_position(0, 0), m_size(0, 0), m_owner(owner){}
  virtual void Draw(r::Render&);
  void CenterOnScreen(const mt::v2i16& ss);
  virtual bool IsModal() const { return true; }
  // IInputTarget
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int id)
  {
    m_container.Input(p - m_position, type, id);
  }
  virtual void Close();
protected:
  f32               m_opacity;
  mt::v2i16         m_position;
  mt::v2u16         m_size;
  ContainerAbsolute m_container;
  r::Frame9         m_bg;
  ScreenManager*    m_owner;
};

class ScreenManager
  : public IInputTarget
{
public:
  void Push(Screen*); // push back
  template<class TS>
  TS* Push(TS* p){
    Push((Screen*)p);
    return p;
  }
  void Pop (Screen* = NULL, bool bdestroy = true); // pop back with check (will be destroyed on DestroyClosed())
  Screen* GetTop() const;
  Screen* GetFirst() const;

  void Draw(r::Render&);
  void Update(f32 /*dt*/);

  // from IInputTarget
  virtual void Input(const mt::v2i16& /*p*/, in::InputEvent::eAction /*type*/, int /*id*/);
  virtual void Acceleration(const mt::v3f& /*a*/);

  ScreenManager();
  virtual ~ScreenManager();

  void DestroyClosed();
private:
  static const u16 sMaxScreens = 10;
  Screen* m_screens[sMaxScreens];
  Screen* m_destroy[sMaxScreens];
  u16     m_count;
  u16     m_dcount;
};

// auto-layout containers
class SidesLayout
  : public Widget
{
public:
  // first draw center, then sides, then corners
  enum eSides
    {
      sCenter       = 0,
      sTop          = 1,
      sLeft         = 2,
      sRight        = 3,
      sBottom       = 4,
      sTopLeft      = 5,
      sTopRight     = 6,
      sBottomLeft   = 7,
      sBottomRight  = 8,
      sMax          = 9
  };
  SidesLayout();
  virtual ~SidesLayout();

  void Set(Widget* w, eSides where);
  template <class W>
  W* Set(W* w, eSides where)
  {
    Set((Widget*)w, where);
    return w;
  }

  void SetFrame(const mt::v2i16& size);
  void Layout();
  void Layout(eSides);

  // from IInputTarget
  virtual void Input(const mt::v2i16& /*p*/, in::InputEvent::eAction /*type*/, int /*id*/);
  virtual void Acceleration(const mt::v3f& /*a*/);

  // from IVisual
  virtual void Draw(r::Render&, const mt::v2i16& /*pivot*/, float /*fOpacity*/);
  virtual mt::v2u16   GetSize() const { return m_size; }
  virtual mt::Recti16 GetRect() const { return mt::Recti16(mt::v2i16(0,0), GetSize()); }

  Widget* GetControl(eSides where)
  {
    return m_widgets[where];
  }

  const Widget* GetControl(eSides where) const
  {
    return m_widgets[where];
  }

  const mt::v2i16& GetPosition(eSides where) const
  {
    EASSERT(m_widgets[where]);
    return m_pos[where];
  }

  virtual bool isIn(const mt::v2i16& p) const;
private:
  Widget*     m_widgets[sMax];
  mt::v2i16   m_pos[sMax];
  mt::v2i16   m_size;
};

class LinearLayout
  : public ContainerAbsolute
{
  DENY_COPY(LinearLayout);
public:
  enum eDirection
  {
    dVertical     = 0,
    dHorizontal   = 1,
    dMax          = 2
  };
  enum eAlign
  {
    aStart  = 0,
    aCenter = 1,
    aEnd    = 2,
    aMax    = 3
  };
  LinearLayout(eDirection, eAlign);

  Widget* Add(Widget* w, i16 gap);
  template <class W>
  W* Add(W* w, i16 gap)
  {
    Add((Widget*)w, gap);
    return w;
  }

  void Layout();

private:
  const eDirection m_direction;
  const eAlign     m_align;
};
NAMESPACE_END(gui)

#endif // GUI_BASE_H
