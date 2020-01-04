#ifndef R_DEVICE_GL_H 
#define R_DEVICE_GL_H 

#include "core/render/r_render.h"
#include "core/common/com_misc.h"
#include "core/res/res_base.h"

#ifdef WIN32
#  include <Windows.h>
#  include <gl\gl.h>
#endif

#ifdef ANDROID_PLATFORM
#  include <GLES/gl.h>
#  include <GLES/glext.h>
#  define OGL_ES
#  include "core/alg/alg_base.h"
#endif

#ifdef IPHONE_PLATFORM
#  include <OpenGLES/ES1/gl.h>
#  include <OpenGLES/ES1/glext.h>
#  define OGL_ES
#endif // TARGET_OS_IPHONE

#ifdef MACOS_PLATFORM
#  include <OpenGL/glu.h>
#endif // MAC_OS

NAMESPACE_BEGIN(r)

#ifdef ANDROID_PLATFORM
class TextureImpl : public alg::INode<TextureImpl>
#else
class TextureImpl 
#endif
{
public:
  TextureImpl(GLuint id, const mt::v2u16& size, const mt::v2u16& rect)
    : m_id(id), m_size(size), m_rect(rect){}
  ~TextureImpl();
  void  Set(u32 stage) const;
  const mt::v2u16& GetSize() const { return m_size; }
  const mt::v2u16& GetRect() const { return m_rect; }
  GLuint getId(){ return m_id; }
  void restore(GLuint newId){m_id = newId;}
private:
  TextureImpl& operator=(const TextureImpl&);
  GLuint m_id;
  const mt::v2u16  m_size;
  const mt::v2u16  m_rect;
};

//////////////////////////////////////////////////////////////////////////
// DeviceImpl

class DeviceImpl
{
public:
	void BeginFrame();
	void EndFrame();

  void Draw(Device::ePrimitiveType, const Material&, const Vertex2f*, const u16* idx, u32 PrimCount);
  void Draw(Device::ePrimitiveType, const Material&, const Vertex3f*, const u16* idx, u32 PrimCount);

  void Clip (const mt::Recti32& r);
  void Set2D();
  void SetModel     (const mt::Matrix4f& pos);
  void SetProjection(const mt::Matrix4f& cam);
  void SetColorMask (bool r, bool g, bool b, bool a);
  void SetClearColor(u32 clr);

  const Texture* GetTexture(res::ResID ID, res::ResourcesPool&);

	bool Init();
	void Release();

  mt::v2i16 GetScreenSize() const;
private:
  void Draw(Device::ePrimitiveType, const Material&, const void* v, const u16* idx, u32 PrimCount, u8 R);

  void SetMaterial(const Material& m);
  void InvalidateMtl(); // invalidate m_setmtl, called every frame

  mt::v4f m_clearclr;

  enum eDrawMode
  {
    dm2D  = 0,
    dm3D  = 1,
    dmMax = 2
  };
  eDrawMode m_drawmode;
  Material  m_setmtl; // current material
};

NAMESPACE_END(r)

#endif // R_DEVICE_GL_H 