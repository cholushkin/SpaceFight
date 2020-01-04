#include "ext/gui/wg_memo.h"
#include "ext/math/mt_colors.h"

NAMESPACE_BEGIN(gui)
using namespace  mt;
Memo::Memo( const Skin& skin )
:  m_skin(skin)
,  m_scrollOffset(0) {
  m_scrollOffset = (f32)GetSize().y;
}

Memo::~Memo() {
  DELETE_VECTOR_ELEMENTS(m_labels);
}

void Memo::Input( const v2i16& /*p*/, in::InputEvent::eAction /*type*/, int /*id*/ ) {
}

void Memo::Draw( r::Render& r, const v2i16& pivot, float fOpacity ) {
  m_skin.m_sheet->Draw( r, m_skin.m_normalFrame, v2f(pivot.x, pivot.y), make_color(fOpacity, 255, 255, 255));

  //const mt::v2u16 memoSize = GetSize();
  Recti16 clpRect = GetRect().Offset(pivot.x,pivot.y);

  r.Clip(Recti32(clpRect.left,clpRect.right,clpRect.top,clpRect.bottom));
  i16 lastPos = (i16)m_scrollOffset;
  for(u32 i=0;i<m_labels.size();++i) {     
    v2i16 dpos = pivot+v2i16(0,lastPos);
    if(!Cropped(dpos.y,dpos.y+m_labels[i]->GetSize().y,pivot)) {
      m_labels[i]->Draw(r,dpos,fOpacity);
      lastPos += m_labels[i]->GetSize().y;
    }
  }
  r.Clip();
}

v2u16 Memo::GetSize() const {
  Rectf rect = m_skin.m_sheet->GetRect(m_skin.m_normalFrame);
  return v2u16((u16)rect.Width(),(u16)rect.Height());
}

void Memo::AddText( const wchar_t* text ) {  
  TextLabel* label = new TextLabel(m_skin.m_font,text,COLOR_WHITE,r::BitmapText::tlLeft,GetSize().x);
  m_labels.push_back(label);

  bool isNeedScroll = true;
  if(isNeedScroll)
    m_scrollOffset -= label->GetSize().y;
}

// void Memo::focusOnLastRow() {
//   m_scrollOffset = (f32)GetSize().y - m_skin.m_font->m_common.lineHeight
// }

void Memo::Scroll( f32 offset ) {
  m_scrollOffset+=offset;
}

bool Memo::Cropped( i16 top, i16 bot, const v2i16& pivot ) const {
  u16 memoBottom = GetSize().y + pivot.y;
  return (top>memoBottom) || (bot<pivot.y);
}
NAMESPACE_END(gui)