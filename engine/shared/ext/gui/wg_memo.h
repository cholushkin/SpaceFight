#ifndef wg_memo_h__
#define wg_memo_h__
#include "gui_base.h"

#include <vector>

NAMESPACE_BEGIN(gui)

class Memo : public Widget {
  DENY_COPY(Memo)
public:
  struct Skin {
    const r::SheetSprite*   m_sheet;
    u16                     m_normalFrame;
    u16                     m_disabledFrame;
    const r::BitmapFont*    m_font;
  };
  Memo( const Skin& skin );
  ~Memo();
  virtual void Input(const mt::v2i16& p, in::InputEvent::eAction type, int /*id*/);
  virtual void Draw(r::Render& r, const mt::v2i16& pivot, float fOpacity);
  virtual mt::v2u16 GetSize() const;

  void AddText( const wchar_t* text );
  void Scroll( f32 offset );
  

private:
  Skin                          m_skin;
  std::vector<TextLabel*>       m_labels;
  f32                           m_scrollOffset;

  bool Cropped( i16 top, i16 bot, const mt::v2i16& pivot ) const;

};

NAMESPACE_END(gui)

#endif // wg_memo_h__