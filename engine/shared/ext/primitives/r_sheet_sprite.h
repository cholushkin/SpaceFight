#ifndef r_sheet_sprite_h__
#define r_sheet_sprite_h__

#include "core/math/mt_base.h"
#include "ext/primitives/r_primitives.h"
#include "core/res/res_base.h"


NAMESPACE_BEGIN(r)

class SheetSprite
{
public:
  struct Frame
  {
    Frame()
    {
      v[0]  = mt::v2f(0.0f, 0.0f);
      v[1]  = mt::v2f(0.0f, 0.0f);
      v[2]  = mt::v2f(0.0f, 0.0f);
      v[3]  = mt::v2f(0.0f, 0.0f);
      uv[0] = g_def_uv[0];
      uv[1] = g_def_uv[1];
      uv[2] = g_def_uv[2];
      uv[3] = g_def_uv[3];
      sheet = 0;
    }

    explicit Frame(f32 w, f32 h, f32 px = 0, f32 py = 0)
    {
      v[0]  = mt::v2f(  - px,   - py);
      v[1]  = mt::v2f(w - px,   - py);
      v[2]  = mt::v2f(w - px, h - py);
      v[3]  = mt::v2f(  - px, h - py);
      uv[0] = g_def_uv[0];
      uv[1] = g_def_uv[1];
      uv[2] = g_def_uv[2];
      uv[3] = g_def_uv[3];
      sheet = 0;
    }

    explicit Frame(const mt::v2f iv[4], const mt::v2f iuv[4])
    {
      v[0]  = iv[0];
      v[1]  = iv[1];
      v[2]  = iv[2];
      v[3]  = iv[3];
      uv[0] = iuv[0];
      uv[1] = iuv[1];
      uv[2] = iuv[2];
      uv[3] = iuv[3];
      sheet = 0;
    }

    explicit Frame(f32 w, f32 h, f32 px, f32 py, const mt::v2f iuv[4])
    {
      v[0]  = mt::v2f(  - px,   - py);
      v[1]  = mt::v2f(w - px,   - py);
      v[2]  = mt::v2f(w - px, h - py);
      v[3]  = mt::v2f(  - px, h - py);
      uv[0] = iuv[0];
      uv[1] = iuv[1];
      uv[2] = iuv[2];
      uv[3] = iuv[3];
      sheet = 0;
    }

    Frame(f32 w, f32 h, f32 px, f32 py, const mt::v2f& iuv0, const mt::v2f& iuv1, const mt::v2f& iuv2, const mt::v2f& iuv3)
    {
      v[0]  = mt::v2f(  - px,   - py);
      v[1]  = mt::v2f(w - px,   - py);
      v[2]  = mt::v2f(w - px, h - py);
      v[3]  = mt::v2f(  - px, h - py);
      uv[0] = iuv0;
      uv[1] = iuv1;
      uv[2] = iuv2;
      uv[3] = iuv3;
      sheet = 0;
    }

    mt::v2f uv[4];
    mt::v2f v [4];
    u16     sheet;
  };

  void Draw(Render& r, u32 frame, const mt::v2f& pos) const;
  void Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color) const;
  void Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color, float fScale) const;
  void Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color, const mt::v2f& scale) const;
  void Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color, float fScale, float fAngle, bool bHQRot) const;
  void Draw(Render& r, u32 frame, const mt::v2f& pos, u32 color, const mt::v2f& scale, float fAngle, bool bHQRot) const;
  void Draw(Render& r, u32 frame, const mt::Matrix3f&  m, u32 color) const;
  void Draw(Render& r, u32 frame, const mt::Matrix23f& m, u32 color) const;

  void Load(const Material& m, Frame* pFrames, u32 nframes); // will own pFrames!       (does not copy)
  void Load(      Material* m, Frame* pFrames, u32 nframes); // will own pFrames and m! (does not copy)
  void DefineFrameNames( char* frameNamesData, const char** frameNames ); // will also own these datum

  bool IsValid() const
  { return m_frames && m_nframes > 0; }

  mt::Rectf GetRect(u32 frame) const
  {
    EASSERT(frame < m_nframes);
    return mt::Rectf(m_frames[frame].v[0], m_frames[frame].v[2]);
  }

  SheetSprite()
    : m_frames (NULL)
    , m_nframes(0)
    , m_image(NULL)
    , m_frameNamesData(NULL)
    , m_frameNames(NULL)
  {}

  SheetSprite(const Material& m, Frame* pFrames, u32 nframes)
    : m_frames (NULL)
    , m_nframes(0)
    , m_image(NULL)
    , m_frameNamesData(NULL)
    , m_frameNames(NULL)
  {
    Load(m, pFrames, nframes);
  }

  ~SheetSprite();

  u32 GetNFrames() const {
    return m_nframes;
  }

  Material& GetMaterial(int sheet = 0){
    return m_image[sheet];
  }

  const Material& GetMaterial(int sheet = 0) const {
    return m_image[sheet];
  }

  const Frame& GetFrame(u32 frame) const {
    EASSERT(m_frames);
    EASSERT(m_nframes > frame);
    return m_frames[frame];
  }

  const char* GetFrameName( u32 frame ) const {
    EASSERT(m_frames);
    EASSERT(m_nframes > frame);
    if (NULL==m_frameNamesData)
      return NULL;
    return m_frameNames[ frame ];
  }

  u32 FindFrameByName(const char* frameName) const {
    for(u32 i = 0; i < m_nframes; ++i)
      if (0==strcmp(m_frameNames[i],frameName) )
        return i;
    return m_nframes;
  }

  static const SheetSprite* Load(const res::ResID sprid, res::ResourcesPool& pool, Render& r);
  static const SheetSprite* Load(const char* szMapFile,  res::ResourcesPool& pool, Render& r);

  static bool Load(SheetSprite& sprite, const char* szMapFile, res::ResourcesPool& pool, Render& r);
  static bool Load(SheetSprite& sprite, res::ResID mapRes,     res::ResourcesPool& pool, Render& r);

  static Frame*              CreateSlicedFrames(u32 w, u32 h, i32 px, i32 py, u16 xs, u16 ys, u16 cnt);
  static const SheetSprite*  CreateSlicedSprite(Render& r, res::ResourcesPool& rpool, res::ResID sprid, res::ResID imgid, u16 xs, u16 ys, u32 w, u32 h, u16 count, i32 px, i32 py);

  static Frame* MirrorUVs  (Frame* p, u32 n, bool h, bool v);
  static Frame* RotateUVs90(Frame* p, u32 n, int times);

private:
  Material*    m_image;
  Frame*       m_frames;
  u32          m_nframes;
  char*        m_frameNamesData;
  const char** m_frameNames;

  bool AssureFrame(u32) const;
};

typedef SheetSprite Sprite;

NAMESPACE_END(r)

#endif // r_sheet_sprite_h__
