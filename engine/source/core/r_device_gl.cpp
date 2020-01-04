#include "r_device_gl.h"
#include "platform.h"
#include "core/res/res_base.h"
#include "ext/math/mt_colors.h"

extern "C" bool RenderInit();
extern "C" void RenderRelease();
extern "C" void RenderSwapBuffers();
extern "C" bool CreateTextureRes(const char* name, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh);

using namespace res;
using namespace mt;
using namespace r;

// OpenGLES defines
#ifndef GL_CLAMP
# define GL_CLAMP GL_CLAMP_TO_EDGE
#endif // GL_CLAMP

#if WIN32 || TARGET_OS_MAC
# define GL_CHECK {const int e = glGetError(); e; EASSERT(0 == e);}
#else // WIN32
# define GL_CHECK
#endif // WIN32

GLuint GL(Material::eComparisons c)
{
  EASSERT(Material::ecNotEqual >= c);
  static const GLuint constants[] =
  {
    GL_NEVER,
    GL_ALWAYS,
    GL_LESS,
    GL_LEQUAL,
    GL_EQUAL,
    GL_GEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
  };
  return constants[c];
}

GLuint GL(Material::StencilState::eStencilOp c)
{
  EASSERT(Material::StencilState::soInvert >= c);
  static const GLuint constants[] =
  {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
  };
  return constants[c];
}


void ZStencil(const Material& m)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if(m.m_stencil.isDefault())
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST); // $$$Ink:should be ON for 3d
  }
  else
  {
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(Material::StencilState::soReplace == m.m_stencil.zpass ? GL_TRUE : GL_FALSE);
    glTranslatef(0.0f, (float)m.m_stencil.ref, 0.0f);
    glDepthFunc(GL(m.m_stencil.fun));
  }
};

//////////////////////////////////////////////////////////////////////////
// TextureImpl

TextureImpl::~TextureImpl()
{
  glDeleteTextures( 1, &m_id );
}

void TextureImpl::Set(u32 /*stage*/) const
{
  glBindTexture(GL_TEXTURE_2D, m_id);
}

//////////////////////////////////////////////////////////////////////////
// DeviceImpl

void DeviceImpl::BeginFrame()
{
  m_drawmode = dmMax;
  InvalidateMtl();
  v2i16 vsOffset = Device::sContext.GetVirtScreenOffset();
  v2i16 vsSz     = Device::sContext.GetVirtScreenSize();

  glViewport( vsOffset.x, vsOffset.y, vsSz.x, vsSz.y );
  glDepthMask(GL_TRUE);
  glEnable(GL_STENCIL_TEST);
  glClearColor(m_clearclr.r, m_clearclr.g, m_clearclr.b, m_clearclr.a); // in case we've lost state
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void DeviceImpl::EndFrame()
{
  //glFlush();
  ::RenderSwapBuffers();
}

void DeviceImpl::SetMaterial(const Material& m)
{
  GL_CHECK;

  if(m.m_blend != m_setmtl.m_blend)
  {
    // Blend
    if(Material::bmAlpha == m.m_blend)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if(Material::bmAdditive == m.m_blend)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else if(Material::bmNone == m.m_blend)
    {
      glDisable(GL_BLEND);
    }
    else
    {
      EALWAYS_ASSERT("Unknown blend mode");
    }
  }

  if(m.m_pTexture != m_setmtl.m_pTexture ||
     m.m_filter   != m_setmtl.m_filter ||
     m.m_wrap     != m_setmtl.m_wrap)
  {
    // Texture
    if(NULL == m.m_pTexture)
      glDisable(GL_TEXTURE_2D);
    else
    {
      if(m.m_pTexture != m_setmtl.m_pTexture)
      {
        glEnable(GL_TEXTURE_2D);
        m.m_pTexture->Set(0);
      }
      if(m.m_filter != m_setmtl.m_filter)
      {
        const GLint filt = (Material::fmNearest == m.m_filter) ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filt);
      }
      if(m.m_wrap != m_setmtl.m_wrap)
      {
        const GLint wrap = (Material::wmClamp   == m.m_wrap  ) ? GL_CLAMP   : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     wrap);
      }
    }
  }

  // Stencil
  if(m.m_stencil.isDefault())
  {
    glDisable(GL_STENCIL_TEST);
    //glDisable(GL_ALPHA_TEST); // should be ON for 3d
  }
  else
  {
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 1/255.0f);
    glStencilFunc(GL(m.m_stencil.fun),      m.m_stencil.ref,       m.m_stencil.mask);
    glStencilOp  (GL(m.m_stencil.sfail), GL(m.m_stencil.zfail), GL(m.m_stencil.zpass));
  }
  GL_CHECK;

  m_setmtl = m;
}

void DeviceImpl::InvalidateMtl()
{
  m_setmtl.m_pTexture = (const Texture*)0xDEADF00D;
  m_setmtl.m_blend  = Material::bmMax;
  m_setmtl.m_wrap   = Material::wmMax;
  m_setmtl.m_filter = Material::fmMax;
}

void DeviceImpl::Clip(const Recti32& r)
{
  GL_CHECK;
  if(r.isValid())
  {
    glEnable(GL_SCISSOR_TEST);
    const v2i16 tl = Device::sContext.UserToPhysical(r.TL().x, r.TL().y);
    const v2i16 br = Device::sContext.UserToPhysical(r.RB().x, r.RB().y);
    const v2i16 ph = Device::sContext.GetPhysScreenSize();
    glScissor(tl.x, ph.y - br.y, br.x - tl.x, br.y - tl.y);
  }
  else
    glDisable(GL_SCISSOR_TEST);
  GL_CHECK;
}

void DeviceImpl::SetColorMask(bool r, bool g, bool b, bool a)
{
  glColorMask(
    r ? GL_TRUE : GL_FALSE,
    g ? GL_TRUE : GL_FALSE,
    b ? GL_TRUE : GL_FALSE,
    a ? GL_TRUE : GL_FALSE
    );
}

void DeviceImpl::SetClearColor(u32 clr)
{
  m_clearclr = mt::v4f((f32)GETR(clr), (f32)GETG(clr), (f32)GETB(clr), (f32)GETA(clr));
  m_clearclr /= 255.0f;
  glClearColor(m_clearclr.r, m_clearclr.g, m_clearclr.b, m_clearclr.a);
}

void DeviceImpl::Set2D()
{
  if(dm2D == m_drawmode)
    return;

  m_drawmode = dm2D;

  GL_CHECK;
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_CULL_FACE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const v2i16 sd = Device::sContext.GetUserScreenSize();
  if(0 == sd.x || 0 == sd.y)
    return;

#ifdef OGL_ES
  glOrthof(0, sd.x, sd.y, 0, 0, 1);
#else // OGL_ES
  glOrtho (0, sd.x, sd.y, 0, 0, 1);
#endif // OGL_ES
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  GL_CHECK;
}

void DeviceImpl::SetModel(const Matrix4f& pos)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf((const GLfloat *)pos.data);
}

void DeviceImpl::SetProjection(const Matrix4f& cam)
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((const GLfloat *)cam.data);
}

void DeviceImpl::Draw(Device::ePrimitiveType type, const Material& m, const Vertex2f* v, const u16* idx, u32 PrimCount)
{
#ifndef WIN32
  if(!m.m_stencil.isDefault() || 
     (m.m_stencil.isDefault() != m_setmtl.m_stencil.isDefault()))
    ZStencil(m);
#endif // WIN32
  Draw(type, m, v, idx, PrimCount, 2);
}

void DeviceImpl::Draw(Device::ePrimitiveType type, const Material& m, const Vertex3f* v, const u16* idx, u32 PrimCount)
{
  if(dm3D != m_drawmode)
  {
    GL_CHECK;
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDisable(GL_STENCIL_TEST);
    m_drawmode = dm3D;
  }
  GL_CHECK;
  Draw(type, m, v, idx, PrimCount, 3);
}

void DeviceImpl::Draw(Device::ePrimitiveType type, const Material& m, const void* v, const u16* idx, u32 PrimCount, u8 R)
{
  GL_CHECK;
  EASSERT(0 != PrimCount);
  EASSERT(2 == R || 3 == R);
  SetMaterial(m);
  const bool isTextured = NULL != m.m_pTexture;

  if(isTextured)
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  const u8* bytes    = (const u8*)v;
  const u32 pos_size = sizeof(f32) * R;
  const u32 stride   = pos_size + sizeof(v2f)+ sizeof(u32);

  glVertexPointer    (R, GL_FLOAT,         stride, bytes);
  if(isTextured)
    glTexCoordPointer(2, GL_FLOAT,         stride, bytes + pos_size);
  glColorPointer     (4, GL_UNSIGNED_BYTE, stride, bytes + pos_size + sizeof(v2f));

  static const GLuint  glconstants[Device::ptMax] = { GL_POINTS, GL_TRIANGLES, GL_LINES };
  static const GLsizei glprimcnt  [Device::ptMax] = { 1, 3, 2 };

  if(idx)
    glDrawElements(glconstants[type], PrimCount * glprimcnt[type],  GL_UNSIGNED_SHORT, (void*)idx);
  else
    glDrawArrays(glconstants[type], 0, PrimCount * glprimcnt[type]);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  if(isTextured)
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  GL_CHECK;
}

const Texture* DeviceImpl::GetTexture(ResID ID, ResourcesPool& p)
{
  Texture* pT = (Texture*)p.Get(ID);
  if(NULL != pT)
    return pT;
  GLuint id = 0;
  const char* name = p.GetNameFromID(ID);
  unsigned int w, h, rw, rh;
  if(NULL == name || !CreateTextureRes(name, &id, &w, &h, &rw, &rh))
    return NULL;
  pT = new Texture(new TextureImpl(id, v2u16((u16)w, (u16)h), v2u16((u16)rw, (u16)rh)));
  return p.PutGeneric(ID, pT);
}

bool DeviceImpl::Init()
{
  if(!RenderInit())
    return false;

  m_clearclr = mt::v4f(0.0f, 0.0f, 0.0f, 0.0f);
  m_drawmode = dmMax;
  //Common init
  glClearColor(m_clearclr.r, m_clearclr.g, m_clearclr.b, m_clearclr.a);
GL_CHECK;
#ifdef WIN32
  glClearDepth(1.0);
  glDisable(GL_POLYGON_SMOOTH);
#endif

  glDepthFunc(GL_LESS);
  glEnable(GL_COLOR_MATERIAL);
  glFrontFace(GL_CW);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LINE_SMOOTH);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  GL_CHECK;
  Set2D();
  return true;
}

void DeviceImpl::Release()
{
  ::RenderRelease();
}

v2i16 DeviceImpl::GetScreenSize() const
{
  return Device::sContext.GetPhysScreenSize();
}
