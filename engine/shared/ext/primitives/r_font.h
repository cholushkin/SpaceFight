#ifndef r_font_h
#define r_font_h

#include "core/math/mt_base.h"
#include "core/common/com_misc.h"
#include "core/render/r_render.h"

NAMESPACE_BEGIN(r)

//////////////////////////////////////////////////////////////////////////
//// Fonts and Text

class Render;
class Texture;

class BitmapFont
{
public:

#pragma pack(push)
#pragma pack(1)

  struct common
  {
    u16 lineHeight;
    u16 base;
    u16 scaleW;
    u16 scaleH;
    u16 pages;
    u8  bitField;
    u8  alphaChnl;
    u8  redChnl;
    u8  greenChnl;
    u8  blueChnl;
  };

  struct character
  {
    u32 id;
    u16 x;
    u16 y;
    u16 width;
    u16 height;
    i16 xoffset;
    i16 yoffset;
    i16 xadvance;
    u8  page;
    u8  chnl;
  };

  struct kerning 
  {
    u32 first;
    u32 second;
    i16 amount;
  };

  struct uv
  {
    mt::v2f m_uv[4];
  };

#pragma pack(pop)

  BitmapFont();
  ~BitmapFont();
  i32 GetCharID(u32 code) const;
  i16 GetKerning(u32 code_l, u32 code_r) const;

  common     m_common;
  character* m_chars;
  u32        m_nchars;
  kerning*   m_kerning;
  u32        m_nkerning;
  uv*        m_uvcache;

  const Texture** m_pTextures;
};

bool LoadFont(BitmapFont& font, const char* szFile, res::ResourcesPool& pool, Render& r);
const BitmapFont* GetFont(res::ResID id, res::ResourcesPool& pool, Render& r);

class BitmapText
{
public:
  enum eAlign
  {
    tlLeft   = 0,
    tlCenter = 1,
    tlRight  = 2,
  };

  BitmapText();
  ~BitmapText();
  void Draw(Render& r) const;
  void Draw(Render& r, const mt::v2f& pos) const;
  void Draw(Render& r, const mt::v2f& pos, f32 scale) const; // slow
  void Draw(Render& r, const mt::Matrix3f& m) const; // slow

  void SetAlign(eAlign);
  void SetText(const wchar_t* str, int wrapWidth = -1, bool isForceWrap = true); // sample of color change: "[c:0xffff0000]red text[c:def], default text color now"
  void SetText(const BitmapFont* pFont, const wchar_t* str, int wrapWidth = -1, bool isForceWrap = true);

  void SetPosition(const mt::v2i16& p);
  const mt::v2i16& GetPosition() const { return m_coords; }

  mt::v2u16  GetSize () const;
  eAlign GetAlign() const { return m_align; }

  void SetColor(u32 c);
  u32  GetColor() const { return m_color; }

  void SetFilter( Material::eFilterMode fm );

  void SetFontScale(f32); // better be called before SetText
private:
  void Clear();
  u32 Format(const wchar_t* str, int wrapWidth, bool isForceWrap);  // align and layout there
  u32 ProcessTag(const wchar_t* str);
  void RecalculateCoords();
  
  const BitmapFont*       m_pFont;

  struct Letter
  {
    mt::v2f coords[4];
    u32     color; // for color tagging
    i32     id;
    u16     line;
    i16     kerning;
    u8      page;
  };
  Letter*                 m_cache;
  u32                     m_length;
  mt::v2i16               m_coords;
  f32                     m_scale;
  mt::v2u16               m_size;
  eAlign                  m_align;
  u32                     m_color;
  Material::eFilterMode   m_filter;
};

NAMESPACE_END(r)

#endif // r_font_h
