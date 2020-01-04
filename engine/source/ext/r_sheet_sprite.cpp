#include "ext/primitives/r_sheet_sprite.h"
#include "core/io/io_base.h"
#include "core/res/res_base.h"
#include "core/alg/alg_base.h"
#include "ext/algorithm/alg_arrays.h"

using namespace res;
using namespace mt;

#pragma warning(disable:4996) // 'strcpy': This function or variable may be unsafe.


NAMESPACE_BEGIN(r)

bool SheetSprite::AssureFrame(u32 frame) const
{
  EASSERT(m_frames);
  if(!m_frames)
    return false;
  EASSERT(m_nframes > frame);
  return (m_nframes > frame);
}

void SheetSprite::Draw( Render& r, u32 frame, const v2f& pos ) const
{
  if(!AssureFrame(frame))
    return;

  const v2f coords[4] =
  {
    m_frames[frame].v[0] + pos,
    m_frames[frame].v[1] + pos,
    m_frames[frame].v[2] + pos,
    m_frames[frame].v[3] + pos,
  };
  r.PostQuad(coords, m_frames[frame].uv, g_def_colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw( Render& r, u32 frame, const v2f& pos, u32 color ) const
{
  if(!AssureFrame(frame))
    return;

  const v2f coords[4] =
  {
    m_frames[frame].v[0] + pos,
    m_frames[frame].v[1] + pos,
    m_frames[frame].v[2] + pos,
    m_frames[frame].v[3] + pos,
  };
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw( Render& r, u32 frame, const v2f& pos, u32 color, float fScale ) const
{
  if(!AssureFrame(frame))
    return;

  const v2f coords[4] =
  {
    fScale * m_frames[frame].v[0] + pos,
    fScale * m_frames[frame].v[1] + pos,
    fScale * m_frames[frame].v[2] + pos,
    fScale * m_frames[frame].v[3] + pos,
  };
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color, const mt::v2f& scale) const
{
  if(!AssureFrame(frame))
    return;

  const v2f coords[4] =
  {
    scale * m_frames[frame].v[0] + pos,
    scale * m_frames[frame].v[1] + pos,
    scale * m_frames[frame].v[2] + pos,
    scale * m_frames[frame].v[3] + pos,
  };
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw( Render& r, u32 frame, const v2f& pos, u32 color, float fScale, float fAngle, bool bHQRot ) const
{
  const v2f scale(fScale,fScale);
  Draw(r,frame,pos,color,scale,fAngle,bHQRot);
}

void SheetSprite::Draw(Render& r, u32 frame, const v2f& pos, u32 color, const v2f& scale, float fAngle, bool bHQRot) const
{
  if(!AssureFrame(frame))
    return;

  const Rotator rot(fAngle, bHQRot);

  const v2f coords[4] = {
    rot.Rotate(m_frames[frame].v[0]*scale) + pos,
    rot.Rotate(m_frames[frame].v[1]*scale) + pos,
    rot.Rotate(m_frames[frame].v[2]*scale) + pos,
    rot.Rotate(m_frames[frame].v[3]*scale) + pos,
  };
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw( Render& r, u32 frame, const mt::Matrix3f& m, u32 color ) const
{
  if(!AssureFrame(frame))
    return;

  v2f coords[4];
  TransformCoord(coords[0], m_frames[frame].v[0], m);
  TransformCoord(coords[1], m_frames[frame].v[1], m);
  TransformCoord(coords[2], m_frames[frame].v[2], m);
  TransformCoord(coords[3], m_frames[frame].v[3], m);
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Draw(Render& r, u32 frame, const mt::Matrix23f& m, u32 color) const
{
  if(!AssureFrame(frame))
    return;

  v2f coords[4];
  TransformCoord(coords[0], m_frames[frame].v[0], m);
  TransformCoord(coords[1], m_frames[frame].v[1], m);
  TransformCoord(coords[2], m_frames[frame].v[2], m);
  TransformCoord(coords[3], m_frames[frame].v[3], m);
  const u32 colors[4] = {color, color, color, color};
  r.PostQuad(coords, m_frames[frame].uv, colors, m_image[m_frames[frame].sheet]);
}

void SheetSprite::Load( const Material& m, Frame* pFrames, u32 nframes )
{
  SAFE_DELETE_ARRAY(m_frames);
  SAFE_DELETE_ARRAY(m_image);
  m_image = new Material[1];
  m_image[0] = m;
  m_frames  = pFrames;
  m_nframes = nframes;
}

void SheetSprite::Load(Material* m, Frame* pFrames, u32 nframes)
{
  SAFE_DELETE_ARRAY(m_frames);
  SAFE_DELETE_ARRAY(m_image);
  m_image   = m;
  m_frames  = pFrames;
  m_nframes = nframes;
}

SheetSprite::~SheetSprite()
{
  SAFE_DELETE_ARRAY(m_frames);
  SAFE_DELETE_ARRAY(m_image);
  SAFE_DELETE_ARRAY(m_frameNamesData);
  SAFE_DELETE_ARRAY(m_frameNames);
  m_nframes = 0;
}

void SheetSprite::DefineFrameNames( char* frameNamesData, const char** frameNames )
{
  SAFE_DELETE_ARRAY(m_frameNamesData);
  SAFE_DELETE_ARRAY(m_frameNames);
  m_frameNamesData = frameNamesData;
  m_frameNames     = frameNames;
}

struct SpriteHeader
{
  u32 sheetcount;
  u32 framecount;
};

struct SheetHeader
{
  u32 sheetwidth;
  u32 sheetheight;
};

struct SpriteFrame
{
  u32 x;        // x on texture
  u32 y;        // x on texture
  u32 width;    // width on texture
  u32 height;   // height on texture
  u32 x_offset; // x offset after trim
  u32 y_offset; // y offset after trim
  u32 original_width;
  u32 original_height;
  i32 pivot_x;  // pivot x for original (non-trimmed) image
  i32 pivot_y;  // pivot y for original (non-trimmed) image
  u32 sheet;
};

bool SheetSprite::Load(Sprite& sprite, ResID mapRes, ResourcesPool& pool, Render& r)
{
  const char* mapFile = ResourcesPool::GetNameFromID(mapRes);
  EASSERT(mapFile);
  return Load(sprite, mapFile, pool, r);
}

bool SheetSprite::Load(Sprite& sprite, const char* szMapFile, ResourcesPool& pool, Render& r) {
  io::FileStream f;
  if(!f.Open(szMapFile, true, true))
    return false;

  char magic[4];
  f.Read(&magic[0], sizeof(magic));

  SpriteHeader header;
  io::Read(f, header);

  alg::ArrayPtr<SheetHeader> pS(new SheetHeader[header.sheetcount]);
  if(!f.Read( (char*)(SheetHeader*)pS, header.sheetcount * sizeof(SheetHeader)))
     return false;

  alg::ArrayPtr<SheetSprite::Frame> pF(new SheetSprite::Frame[header.framecount]);
  SpriteFrame fd; // frame data
  for(u32 i = 0; header.framecount != i; ++i) {
    if(!io::Read(f, fd))
      return false;
    const f32 l_x = fd.x / (f32)pS[fd.sheet].sheetwidth;
    const f32 t_y = fd.y / (f32)pS[fd.sheet].sheetheight;
    const f32 r_x = (fd.x + fd.width ) / (f32)pS[fd.sheet].sheetwidth;
    const f32 b_y = (fd.y + fd.height) / (f32)pS[fd.sheet].sheetheight;
    pF[i] = SheetSprite::Frame(
      (f32)fd.width, (f32)fd.height,
      (f32)fd.pivot_x - fd.x_offset, (f32)fd.pivot_y - fd.y_offset,
      v2f(l_x, t_y),
      v2f(r_x, t_y),
      v2f(r_x, b_y),
      v2f(l_x, b_y));
    pF[i].sheet = (u16)fd.sheet;
  }


  // read list of frame names
  if( !f.isEOF() ) 
  {
    i16 frameNamesDataSize = 0;
    f.Read16(frameNamesDataSize);
    EASSERT(frameNamesDataSize>0);
    alg::ArrayPtr<char>        frameNamesData(new char[frameNamesDataSize + 1]); // + 1 for additional zero
    alg::ArrayPtr<const char*> frameNames    (new const char*[header.framecount]);
    f.Read(frameNamesData,frameNamesDataSize+1);

    size_t frame = 0;
    char* pn = &frameNamesData[0];
    frameNames[frame++] = pn;
    for(; header.framecount != frame; ++frame)
    {
      while(*(pn++));
      frameNames[frame] = pn;
    }
    sprite.DefineFrameNames(frameNamesData.withdraw(), frameNames.withdraw());
  }

  // $$$ Total shit goes there
  char szImg[128] = {0};

  strncpy(szImg, szMapFile, ARRAY_SIZE(szImg));
  char* numpos = strchr(szImg, '.'); EASSERT(numpos);
  strncpy(numpos, "_0.png", ARRAY_SIZE(szImg) - (numpos - szImg));
  ++numpos;

  alg::ArrayPtr<Material> pM(new Material[header.sheetcount]);
  for(unsigned int i = 0; header.sheetcount != i; ++i)
  {
    *numpos = (char)('0' + i); // only up to 10 sheets now
    const ResID id = ResourcesPool::GetIDFromName(szImg);
    EASSERT(ResourcesPool::s_invalidID != id);
    if(ResourcesPool::s_invalidID == id)
      return false;
    const Texture* pT = r.GetTexture(id, pool);
    EASSERT(pT);
    pM[i].m_pTexture = pT;
  }

  sprite.Load(pM.withdraw(), pF.withdraw(), header.framecount);
  return true;
}

const SheetSprite* SheetSprite::Load(const res::ResID sprid, res::ResourcesPool& pool, Render& r)
{
  if (r::SheetSprite* spr = (r::SheetSprite*)pool.Get(sprid))
    return spr;
  return Load(res::ResourcesPool::GetNameFromID(sprid), pool, r);
}

const SheetSprite* SheetSprite::Load(const char* szMapFile, res::ResourcesPool& pool, Render& r)
{
  const res::ResID id = res::ResourcesPool::GetIDFromName(szMapFile);
  EASSERT(res::ResourcesPool::s_invalidID != id);
  r::Sprite* spr = (r::Sprite*)pool.Get(id);
  if (spr)
    return spr;
  spr = new Sprite();
  if(Load( *spr, szMapFile, pool, r))
    return pool.PutGeneric(id, spr);
  delete spr;
  return NULL;
}

//////////////////////////////////////////////////////////////////////////
// Sprite utility

Sprite::Frame* SheetSprite::CreateSlicedFrames(u32 w, u32 h, i32 px, i32 py, u16 xs, u16 ys, u16 cnt )
{
  Sprite::Frame* pF = new Sprite::Frame[xs * ys];
  for(u16 y = 0; ys != y && 0 != cnt; ++y)
  {
    for(u16 x = 0; xs != x && 0 != cnt ; ++x)
    {
      f32 lx =  x      * 1.0f / xs;
      f32 rx = (x + 1) * 1.0f / xs;
      f32 ty =  y      * 1.0f / ys;
      f32 by = (y + 1) * 1.0f / ys;
      const mt::v2f iuv[4] = {v2f(lx, ty), v2f(rx, ty), v2f(rx, by), v2f(lx, by)};
      pF[y * xs + x] = Sprite::Frame((f32)w, (f32)h, (f32)px, (f32)py, iuv);
    }
  }
  return pF;
}

const Sprite* SheetSprite::CreateSlicedSprite(Render& r, ResourcesPool& rpool, ResID sprid, ResID imgid, u16 xs, u16 ys, u32 w, u32 h, u16 count, i32 px, i32 py)
{
  Sprite* ps = (Sprite*)rpool.Get(sprid);
  if(NULL != ps)
    return ps;
  Material btn(r.GetTexture(imgid, rpool), Material::bmAlpha, Material::wmClamp);
  ps = new Sprite(btn, CreateSlicedFrames(w, h, px, py, xs, ys, count), count);  
  return rpool.PutGeneric(sprid, ps);
}

Sprite::Frame* SheetSprite::MirrorUVs(Sprite::Frame* p, u32 n, bool h, bool v)
{
  if(!h && !v)
    return p;
  for(u32 f = 0; n != f; ++f)
  {
    if(h)
    {
      std::swap(p[f].uv[0].x, p[f].uv[1].x);
      std::swap(p[f].uv[2].x, p[f].uv[3].x);
    }
    if(v)
    {
      std::swap(p[f].uv[0].y, p[f].uv[3].y);
      std::swap(p[f].uv[1].y, p[f].uv[2].y);
    }
  }
  return p;
}

Sprite::Frame* SheetSprite::RotateUVs90(Sprite::Frame* p, u32 n, int times)
{
  if(0 == times % 4)
    return p;
  for(u32 f = 0; n != f; ++f){
    alg::rotate(&p[f].uv[0], 4, times);
    alg::rotate(&p[f].v [0], 4, times);
  }
  return p;
}

NAMESPACE_END(r)
