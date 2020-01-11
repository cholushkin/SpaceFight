#include "ext/primitives/r_font.h"
#include "ext/algorithm/alg_arrays.h"
#include "core/render/r_render.h"
#include "core/io/io_base.h"
#include "core/res/res_base.h"
#include "ext/math/mt_colors.h"


using namespace res;
using namespace mt;
using namespace alg;

NAMESPACE_BEGIN(r)

//////////////////////////////////////////////////////////////////////////
// font
BitmapFont::BitmapFont()
: m_chars(NULL)
, m_nchars(0)
, m_kerning(NULL)
, m_nkerning(0)
, m_uvcache(NULL)
, m_pTextures(NULL)
{
}

BitmapFont::~BitmapFont()
{
  SAFE_DELETE_ARRAY(m_chars);
  SAFE_DELETE_ARRAY(m_kerning);
  SAFE_DELETE_ARRAY(m_uvcache);
  SAFE_DELETE_ARRAY(m_pTextures);
}

inline bool operator<(u32 code, const BitmapFont::character& c)
{
  return code < c.id;
}

inline bool operator<(const BitmapFont::character& c, u32 code)
{
  return c.id < code;
}

i32 BitmapFont::GetCharID(u32 code) const
{
  return array_binary_search_key_cmp(&m_chars[0], 0, m_nchars - 1, code);
}

inline bool operator<(const BitmapFont::kerning& k, const v2u32& p)
{
  if(k.first != p.x)
    return k.first < p.x;
  return k.second < p.y;
}

inline bool operator<(const v2u32& p, const BitmapFont::kerning& k)
{
  if(k.first != p.x)
    return p.x < k.first;
  return p.y < k.second;
}

i16 BitmapFont::GetKerning(u32 code_l, u32 code_r) const
{
  const i32 i = array_binary_search_key_cmp(&m_kerning[0], 0, m_nkerning - 1, v2u32(code_l, code_r));
  if(-1 == i)
    return 0;
  return m_kerning[i].amount;
}


const Texture* GetFontTexture(const char* szName, ResourcesPool& pool, Render& r)
{
  ResID id = ResourcesPool::GetIDFromName(szName);
  EASSERT(ResourcesPool::s_invalidID != id);
  if(ResourcesPool::s_invalidID == id)
    return NULL;
  return r.GetTexture(id, pool);
}

const BitmapFont* GetFont(ResID id, ResourcesPool& pool, Render& r)
{
  BitmapFont* res = (BitmapFont*)pool.Get(id);
  if(NULL != res)
    return res;
  const char* szFile = ResourcesPool::GetNameFromID(id);
  if(NULL == szFile)
    return NULL;
  res = new BitmapFont();
  if(!LoadFont(*res, szFile, pool, r))
  {
    SAFE_DELETE(res);
    return NULL;
  }
  return pool.PutGeneric(id, res);
}

bool LoadFont(BitmapFont& font, const char* szFile, ResourcesPool& pool, Render& r)
{
  EASSERT(NULL == font.m_chars);
  EASSERT(NULL == font.m_kerning);
  EASSERT(NULL == font.m_uvcache);
  io::FileStream f;
  if(!f.Open(szFile, true, true))
    return false;

  u8 header[4];
  if(!f.Read((char*)&header[0], 4))
    return false;

  u8  blockid = 0;
  u32 block_size = 0;
  
  // info
  if(!f.Read8(blockid))
    return false;
  EASSERT(1 == blockid);
  if(!f.Read32(block_size))
    return false;
  if(!f.Seek(block_size))
    return false;

  // common
  if(!f.Read8(blockid))
    return false;
  EASSERT(2 == blockid);
  if(!f.Read32(block_size))
    return false;

  EASSERT(block_size == sizeof(font.m_common));
  if(!f.Read((char*)&font.m_common,  sizeof(font.m_common)))
    return false;

  // pages
  if(!f.Read8(blockid))
    return false;
  EASSERT(3 == blockid);
  if(!f.Read32(block_size))
    return false;

  // get first page name
  char szPagename[256];
  int pagelen = 0;
  
  do
  {
    f.Read(&szPagename[pagelen], 1);
    ++pagelen;
  }
  while(szPagename[pagelen - 1]);
  EASSERT(0 == block_size % pagelen)
  int npages = block_size / pagelen;
  font.m_pTextures = new const Texture* [npages];
  int page = 0;
  font.m_pTextures[page] = GetFontTexture(szPagename, pool, r);
  ++page;
  for(;npages != page; ++page)
  {
    f.Read(&szPagename[0], pagelen);
    font.m_pTextures[page] = GetFontTexture(szPagename, pool, r);
  }

  // characters
  if(!f.Read8(blockid))
    return false;
  EASSERT(4 == blockid);
  if(!f.Read32(block_size))
    return false;
  EASSERT(0 == block_size % sizeof(BitmapFont::character));
  font.m_nchars = block_size / sizeof(BitmapFont::character);
  font.m_chars = new BitmapFont::character[font.m_nchars];
  if(!f.Read((char*)&font.m_chars[0],  font.m_nchars * sizeof(BitmapFont::character)))
    return false;

  font.m_uvcache = new BitmapFont::uv[font.m_nchars];
  // const v2i16 def_uv[4] = {v2i16( 0, 0),v2i16( 1, 0),v2i16( 1, 1), v2i16( 0, 1)};
  float fx = 1.0f / font.m_common.scaleW;
  float fy = 1.0f / font.m_common.scaleH;
  for(size_t c = 0; font.m_nchars != c;++c)
  {
    font.m_uvcache[c].m_uv[0].x = fx * (font.m_chars[c].x);
    font.m_uvcache[c].m_uv[0].y = fy * (font.m_chars[c].y + font.m_chars[c].height);

    font.m_uvcache[c].m_uv[1].x = fx * (font.m_chars[c].x + font.m_chars[c].width);
    font.m_uvcache[c].m_uv[1].y = fy * (font.m_chars[c].y + font.m_chars[c].height);

    font.m_uvcache[c].m_uv[2].x = fx * (font.m_chars[c].x + font.m_chars[c].width);
    font.m_uvcache[c].m_uv[2].y = fy * (font.m_chars[c].y);

    font.m_uvcache[c].m_uv[3].x = fx * (font.m_chars[c].x);
    font.m_uvcache[c].m_uv[3].y = fy * (font.m_chars[c].y);
  }

  // kerning
  if(!f.Read8(blockid))
    return true; // it is OK
  EASSERT(5 == blockid);
  if(!f.Read32(block_size))
    return false;
  EASSERT(0 == block_size % sizeof(BitmapFont::kerning));
  font.m_nkerning = block_size / sizeof(BitmapFont::kerning);
  font.m_kerning = new BitmapFont::kerning[font.m_nkerning];
  if(!f.Read((char*)&font.m_kerning[0],  font.m_nkerning * sizeof(BitmapFont::kerning)))
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////////
// text

BitmapText::BitmapText()
: m_pFont(NULL)
, m_cache(NULL)
, m_length(0)
, m_color(mt::COLOR_WHITE)
, m_align(tlLeft)
, m_filter(Material::fmNearest)
, m_scale(1.0f)
{
}

BitmapText::~BitmapText()
{
  Clear();
}

void BitmapText::Clear()
{
  SAFE_DELETE_ARRAY(m_cache);
  m_length = 0;
  m_size.x = m_size.y = 0;
}

void BitmapText::Draw(Render& r) const
{
  if(!m_pFont || !m_pFont->m_pTextures || !m_cache )
    return;

  Material m;
  m.m_filter = m_filter;
  for(u32 c = 0; m_length != c; ++c)
  {
    const u32 color[4] = {m_cache[c].color, m_cache[c].color, m_cache[c].color, m_cache[c].color};
    m.m_pTexture = m_pFont->m_pTextures[m_cache[c].page];
    r.PostQuad(m_cache[c].coords, m_pFont->m_uvcache[m_cache[c].id].m_uv,color, m);
  }
}

void BitmapText::Draw(Render& r, const v2f& pos) const
{
  if(!m_pFont || !m_pFont->m_pTextures || !m_cache )
    return;

  Material m;
  m.m_filter = m_filter;
  v2f coords[4];
  for(u32 c = 0; m_length != c; ++c)
  {
    const u32 color[4] = {m_cache[c].color, m_cache[c].color, m_cache[c].color, m_cache[c].color};
    m.m_pTexture = m_pFont->m_pTextures[m_cache[c].page];
    coords[0] = m_cache[c].coords[0] + pos;
    coords[1] = m_cache[c].coords[1] + pos;
    coords[2] = m_cache[c].coords[2] + pos;
    coords[3] = m_cache[c].coords[3] + pos;
    r.PostQuad(coords, m_pFont->m_uvcache[m_cache[c].id].m_uv,color, m);
  }
}

void BitmapText::Draw(Render& r, const mt::v2f& pos, f32 scale) const
{
  if(!m_pFont || !m_pFont->m_pTextures || !m_cache )
    return;

  Material m;
  m.m_filter = m_filter;
  v2f coords[4];
  for(u32 c = 0; m_length != c; ++c)
  {
    const u32 color[4] = {m_cache[c].color, m_cache[c].color, m_cache[c].color, m_cache[c].color};
    m.m_pTexture = m_pFont->m_pTextures[m_cache[c].page];
    coords[0] = m_cache[c].coords[0] * scale + pos;
    coords[1] = m_cache[c].coords[1] * scale + pos;
    coords[2] = m_cache[c].coords[2] * scale + pos;
    coords[3] = m_cache[c].coords[3] * scale + pos;
    r.PostQuad(coords, m_pFont->m_uvcache[m_cache[c].id].m_uv,color, m);
  }
}

void BitmapText::Draw(Render& r, const mt::Matrix3f& m) const
{
  if(!m_pFont || !m_pFont->m_pTextures || !m_cache )
    return;

  Material mt;
  mt.m_filter = m_filter;
  v2f coords[4];
  for(u32 c = 0; m_length != c; ++c)
  {
    const u32 color[4] = {m_cache[c].color, m_cache[c].color, m_cache[c].color, m_cache[c].color};
    mt.m_pTexture = m_pFont->m_pTextures[m_cache[c].page];
    TransformCoord(coords[0], m_cache[c].coords[0], m);
    TransformCoord(coords[1], m_cache[c].coords[1], m);
    TransformCoord(coords[2], m_cache[c].coords[2], m);
    TransformCoord(coords[3], m_cache[c].coords[3], m);
    r.PostQuad(coords, m_pFont->m_uvcache[m_cache[c].id].m_uv,color, mt);
  }
}

v2u16 BitmapText::GetSize() const
{
  return m_size;
}

void BitmapText::RecalculateCoords()
{
  const v2u16 size = GetSize();

  for( u32 i = 0; i != m_length; )
  {
    u32 r = i + 1;
    for( u32 const line = m_cache[i].line; r != m_length && line == m_cache[r].line; ++r)
      ;
    
    float x       = m_coords.x;
    const float y = m_coords.y + m_cache[i].line * m_scale * m_pFont->m_common.lineHeight;
    float width   = 0.0f;
    for( u32 l = i; l != r; ++l)
    {
      BitmapFont::character const& c = m_pFont->m_chars[ m_cache[l].id ];

      const i16 kering = m_cache[l].kerning;

      x     += m_scale * kering;
      width += m_scale * kering;

      m_cache[l].coords[0] = v2f(x + m_scale * c.xoffset,                     y + m_scale * c.yoffset + m_scale * c.height);
      m_cache[l].coords[1] = v2f(x + m_scale * c.xoffset + m_scale * c.width, y + m_scale * c.yoffset + m_scale * c.height);
      m_cache[l].coords[2] = v2f(x + m_scale * c.xoffset + m_scale * c.width, y + m_scale * c.yoffset);
      m_cache[l].coords[3] = v2f(x + m_scale * c.xoffset,                     y + m_scale * c.yoffset);

      x += m_scale * c.xadvance;

      if( i == l )
        width += m_scale * (c.xadvance - c.xoffset);
      else
      if( l == r - 1)
        width += m_scale * (c.xoffset + c.width);
      else
        width += m_scale * c.xadvance;
    }

    float offset = 0;
    if( tlCenter == m_align )
      offset = floor( 0.5f * fabs(size.x - width) - 0.5f * size.x );
    if( tlRight == m_align )
      offset = floor( fabs(size.x - width) - size.x );

    for( u32 l = i; l != r; ++l)
      for( u32 c = 0; c < ARRAY_SIZE(m_cache[l].coords); ++c )
        m_cache[l].coords[c].x += offset;

    i = r;
  }
}

void BitmapText::SetAlign(eAlign align)
{
  if( m_align == align )
    return;

  m_align = align;
  RecalculateCoords();
}

void BitmapText::SetPosition(const v2i16& p)
{
  const v2f d(float(p.x - m_coords.x), float(p.y - m_coords.y));
  for(size_t c = 0; m_length != c ; ++c)
    for (size_t i = 0; 4 != i; ++i)
      m_cache[c].coords[i] += d;
  m_coords = p;
}

void BitmapText::SetFontScale(f32 s)
{
  EASSERT(m_scale > mt::epsilon);
  m_size  = mt::v2f(m_size) * s / m_scale;
  m_scale = s;
  RecalculateCoords();
}

void BitmapText::SetColor(u32 color)
{
  m_color = color;
  for(size_t c = 0; m_length != c ; ++c)
      m_cache[c].color = m_color;
}

void BitmapText::SetText(const wchar_t* str, int wrapWidth, bool isForceWrap )
{
  EASSERT(m_pFont);
  if(!m_pFont)
    return;
  Format(str, wrapWidth, isForceWrap);
}

void BitmapText::SetText(const BitmapFont* pFont, const wchar_t* str, int wrapWidth, bool isForceWrap )
{
  m_pFont = pFont;
  Format(str, wrapWidth, isForceWrap);
}

static inline u32 Len(const wchar_t* str)
{
  u32 res(0);
  for(;*str;++str)
    if(L'\n' != *str)
      ++res;
  return res;
}

u32 BitmapText::Format(const wchar_t* str, int wrapWidth, bool isForceWrap)
{
  EASSERT(m_pFont);
  Clear();

  if(0 < wrapWidth)
    wrapWidth = int(wrapWidth / m_scale);

  i32  x       = m_coords.x;

  m_length     = Len(str);
  m_cache      = new Letter[m_length];

  u32  used    = 0;
  u16  lines   = 0;
  u16  width   = 0;
  bool newLine = false;

  u32  wordStartIdx = 0;
  i32  wordStartPos = m_coords.x;
  u32  wordStartPIdx = 0;
  u32 wrapWPosIdx = 0;
  
  for(u32 p = 0 ;str[p]; ++p)
  {
    const bool isNewLine = L'\n' == str[p];
    const i32  id        = m_pFont->GetCharID(str[p]);

    if(-1 == id && !isNewLine )
      continue;

    newLine = isNewLine;
    if( isNewLine )
    {
      x  = m_coords.x;
      ++lines;
      continue;
    }

    const i16 kering = (0 != p && !newLine && !isNewLine ) ? m_pFont->GetKerning(str[p-1], str[p]) : 0;
    x += kering;

    const BitmapFont::character& c = m_pFont->m_chars[id];
    m_cache[used].id        = id; 
    m_cache[used].color     = m_color;
    m_cache[used].page      = c.page;
    m_cache[used].kerning   = kering;
    x                      += c.xadvance;
    
    if( (0 == p) || (L'\n' == str[p - 1]) || (L' ' == str[p - 1]) )
    {
      wordStartPos = x;
      wordStartIdx = used;
      wordStartPIdx = p;
    }

    if( 0 < wrapWidth && (m_coords.x + wrapWidth) < x && m_coords.x < wordStartPos )
    {
      bool doWrap = true;
      if(wrapWPosIdx == wordStartIdx) // try to wrap again same world
      {
        if(isForceWrap)
        {
          //wordStartPos = x;
          EASSERT(used>0);
          wordStartIdx = used-1;
          wordStartPIdx = p;
        }
        else
          doWrap = false;        
      }
      
      if(doWrap)
      {
        wrapWPosIdx = wordStartIdx;
        for( u32 i = wordStartIdx; i < used; ++i )
          ++m_cache[i].line;
        used = wordStartIdx;
        p    = wordStartPIdx;
        x    = m_coords.x;
        ++lines;
      }
    }
    m_cache[used].line      = lines;
    
    width                   = Max<i16>(width, (i16)(x - m_coords.x) );
    ++used;
  }
  
  m_size.x = u16(0 < used ? m_scale * width                                      : 0);
  m_size.y = u16(0 < used ? m_scale * (lines + 1) * m_pFont->m_common.lineHeight : 0);
  m_length = used;

  RecalculateCoords();
  return lines + 1;
}


u32 BitmapText::ProcessTag( const wchar_t* /*str*/ )
{
  return 0;
}

void BitmapText::SetFilter( Material::eFilterMode fm)
{
  m_filter = fm;  
}

NAMESPACE_END(r)