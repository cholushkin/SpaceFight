#ifndef _r_render_h
#define _r_render_h

#include "core/math/mt_base.h"
#include "core/common/com_misc.h"
#include "core/res/res_base.h"

namespace res 
{
  class ResourcesPool;
}

NAMESPACE_BEGIN(r)

class  Texture;
struct Material;
struct Vertex2f;

class DeviceImpl;
class TextureImpl;
struct Vertex2f;
struct Vertex3f;
class Context;


class Texture
{
public:
  Texture(TextureImpl* pimpl) : m_pimpl(pimpl) {}
  ~Texture();
  void Set(u32 stage) const;
  const mt::v2u16& GetSize() const; // of bitmap
  const mt::v2u16& GetRect() const; // of texture (can be bigger than GetSize)
  const mt::v2f    GetUV  () const
  {
    const mt::v2u16& s = GetSize();
    const mt::v2u16& r = GetRect();
    return mt::v2f(s.x/(f32)r.x, s.y/(f32)r.y);
  }
private:
  TextureImpl* m_pimpl;
};

class Device // hardware specific
{
public:
  void BeginFrame();
  void EndFrame();
  enum ePrimitiveType
  {
    ptPoints       = 0,
    ptTriangles    = 1,
    ptLines        = 2,
    ptMax          = 3
  };
  void Draw(ePrimitiveType, const Material&, const Vertex2f*, const u16* idx, u32 PrimCount);
  void Draw(ePrimitiveType, const Material&, const Vertex3f*, const u16* idx, u32 PrimCount);

  void Clip          (const mt::Recti32& r);
  void Set2D         ();
  void SetModel      (const mt::Matrix4f& pos);
  void SetProjection (const mt::Matrix4f& cam);
  void SetColorMask  (bool r, bool g, bool b, bool a);
  void SetClearColor (u32 clr);

  mt::v2i16 GetScreenSize() const;

  static Context sContext;

  const Texture* GetTexture(res::ResID ID, res::ResourcesPool&);
  bool Init();
  void Release();
  Device(): m_impl(NULL) {}
private:
  DeviceImpl* m_impl;
};

//////////////////////////////////////////////////////////////////////////
// Render

struct Material
{
  enum eBlendMode
  {
    bmNone      = 0,
    bmAdditive  = 1,
    bmAlpha     = 2,
    bmMax       = 3,
  };

  enum eWrapMode
  {
    wmClamp      = 0,
    wmRepeat     = 1,
    wmMax        = 2,
  };

  enum eFilterMode
  {
    fmNearest   = 0,
    fmLinear    = 1,
    fmMax       = 2,
  };

  enum eComparisons
  {
    ecNever    = 0, //  aways fails
    ecAlways   = 1, //  always passes
    ecLess     = 2, //  passes if reference value is less than buffer
    ecLEqual   = 3, //  passes if reference value is less than or equal to buffer
    ecEqual    = 4, //  passes if reference value is equal to buffer
    ecGEqual   = 5, //  passes if reference value is greater than or equal to buffer
    ecGreater  = 6, //  passes if reference value is greater than buffer
    ecNotEqual = 7, //  passes if reference value is not equal to buffer
    ecMax      = 8,
  };

  struct StencilState
  {
    enum eStencilOp
    {
      soKeep    = 0, // stencil value unchanged
      soZero    = 1, // stencil value set to zero
      soReplace = 2, // stencil value replaced by stencil reference value
      soIncr    = 3, // stencil value incremented
      soDecr    = 4, // stencil value decremented
      soInvert  = 5, // stencil value bitwise inverted
      soMax     = 6,
    };

    StencilState()
      : sfail(soKeep)
      , zfail(soKeep)
      , zpass(soKeep)
      , fun(ecAlways)
      , ref     (0x0)
      , mask   (0xFF){}
    // StencilOp
    eStencilOp   sfail;
    eStencilOp   zfail;
    eStencilOp   zpass;

    // StencilFunc
    eComparisons fun;
    u8           ref;
    u8           mask;

    bool operator !=(const StencilState& r) const {
      return
        sfail != r.sfail ||
        zfail != r.zfail ||
        zpass != r.zpass ||
        fun   != r.fun   ||
        ref   != r.ref   ||
        mask  != r.mask;
    }
    bool isDefault() const
    {
      static const StencilState ss;
      return !(ss != *this);
    }
  };

  const Texture* m_pTexture;
  eBlendMode     m_blend;
  eWrapMode      m_wrap;
  eFilterMode    m_filter;
  StencilState   m_stencil;

  Material()
    : m_pTexture(NULL), m_blend(bmAlpha), m_wrap(wmRepeat), m_filter(fmNearest) {}

  explicit Material(const Texture* pTexture, eBlendMode blend = bmAlpha, eWrapMode wrap = wmRepeat, eFilterMode filter = fmNearest)
    : m_pTexture(pTexture),  m_blend(blend), m_wrap(wrap), m_filter(filter) {}

  bool operator !=(const Material& r) const {
    return 
      m_pTexture != r.m_pTexture || 
      m_blend    != r.m_blend || 
      m_wrap     != r.m_wrap || 
      m_stencil  != r.m_stencil ||
      m_filter   != r.m_filter;
  }

  Material& operator=(const Material& r) {
    m_pTexture = r.m_pTexture;
    m_blend    = r.m_blend;
    m_wrap     = r.m_wrap;
    m_stencil  = r.m_stencil;
    m_filter   = r.m_filter;
    return *this;
  }

  Material(const Material& r) {
    *this = r;
  }
};

struct Vertex2f
{
  Vertex2f(){}

  Vertex2f(const mt::v2f& coords, const mt::v2f& uv, u32 color)
    : coords(coords), uv(uv), color(color) {}
  mt::v2f  coords;
  mt::v2f  uv;
  u32  color;
};

struct Vertex3f
{
  Vertex3f(){}
  Vertex3f(const mt::v3f& coords, const mt::v2f& uv, u32 color)
    : coords(coords), uv(uv), color(color) {}
  mt::v3f    coords;
  mt::v2f    uv;
  u32    color;
};

struct SubMesh
{
  SubMesh()
    : m_v(NULL), m_i(NULL)
    , m_nv(), m_ni(0){}
  Vertex3f* m_v; // does not own!
  u16*      m_i; // does not own!
  u16       m_nv;
  u16       m_ni;
  Material  m_mat;
};

struct Mesh
{
  Mesh()
    : m_meshes(0),m_nmeshes(0), m_v(0),m_i(0) {}
  ~Mesh();
  SubMesh* m_meshes;
  u16      m_nmeshes;
  Vertex3f* m_v;
  u16*      m_i;
};

class Context 
{
  DENY_COPY(Context);
public:  
  enum eScreenFitMode
  {
    sfFitFullScreen         = 0, 
    sfFitByScreenWidth      = 1,
    sfFitByScreenHeight     = 2,
    sfNoScale               = 3,
    sfStretch               = 4,
    sfCount     
  };

  Context(const mt::v2i16& pss, const mt::v2i16& uss, eScreenFitMode sfm = sfNoScale);
  Context();
  void SetPhysScreenSize(const mt::v2i16& pss);
  void SetUserScreenSize(const mt::v2i16& uss, eScreenFitMode sfm);

  const mt::v2i16& GetPhysScreenSize  () const {return m_physScreenSize;}
  const mt::v2i16& GetVirtScreenOffset() const {return m_virtScreenOffset;}
        mt::v2i16  GetVirtScreenSize  () const {return m_virtScreenSize;}
  const mt::v2i16& GetUserScreenSize  () const {return m_userScreenSize;}

  void MapInput( int& x, int& y) const;
  bool IsInsideVirtualScreen(const mt::v2i16& pos) const;

  mt::v2i16 UserToPhysical(int x, int y) const;
protected:
  eScreenFitMode      m_fit;
  mt::v2i16           m_virtScreenOffset;
  mt::v2i16           m_physScreenSize;
  mt::v2i16           m_userScreenSize;
  mt::v2f             m_virtScreenSize;
  f32                 m_usrAspectRatio;
  void ComputeVirtualScreen();
  void FitByWidth();
  void FitByHeight();
  void FitStretch();
  void FitNoScale();
  void clampToUsrScreen(mt::v2i16& coord) const;
};

class Render
{
public:
  Render();

  void Clear();
  void SetClearColor(u32 clr);
  void Clip(const mt::Recti32& r = mt::Recti32(0,-1,0,-1));
  void SetColorMask(bool r, bool g, bool b, bool a);
  void PostQuad(const mt::v2f coords[4], const mt::v2f uv[4], const u32 colors[4], const Material& m);
  void PostTri (const mt::v2f coords[3], const mt::v2f uv[3], const u32 colors[3], const Material& m);
  void PostLine(const mt::v2f coords[2], const u32 colors[2], const Material& m);
  void RenderMesh(const Mesh& m, const mt::Matrix4f& pos, const mt::Matrix4f& cam);
  void FlushDIPs();
  void Present();

  bool Init();
  void Release();
  
  mt::v2i16 GetScreenSize() const;

  const Texture* GetTexture(res::ResID ID, res::ResourcesPool&); // must be 'GetMaterial'
private:
  void SetModel(const mt::Matrix4f& pos);
  void SetProjection (const mt::Matrix4f& cam);
  struct DIP
  {
    Material                material;
    u32                     primcnt;
    Device::ePrimitiveType  type;
    DIP()
      : primcnt(0)
      , type(Device::ptMax)
    {}
  };

  static const u32 s_vsize_max = 1000;
  static const u32 s_isize_max = s_vsize_max * 6 / 4; // assume quads

  DIP      m_dip;
  Vertex2f m_vb[s_vsize_max];
  u16      m_ib[s_isize_max];
  u32      m_vsize_curr;
  u32      m_isize_curr;
  Device   m_device;
};

NAMESPACE_END(r)

#endif // _r_render_h