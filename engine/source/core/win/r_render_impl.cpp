//////////////////////////////////////////////////////////////////////////
//
// r_render.cpp 
// render implementation
// Tinte, 2008
//
// Win32
//////////////////////////////////////////////////////////////////////////

#include "core/common/com_misc.h"
#include "core/math/mt_base.h"
#include <windows.h>
#include <gl\gl.h>
#include <vector>
#include "../loaders/stb_image.h"
#include "../loaders/r_tex_dds.h"
#include "../loaders/r_tex_webp.h"

extern HWND g_hWnd;
HGLRC   g_hrc;		//GLRenderContext
HDC     g_hdc;		//Device Context
GLuint	g_PixelFormat;


using namespace mt;

class GLContext
{
public:

  GLContext()
  {
    reset();
  }

  ~GLContext()
  {
    purge();
  }

  void init(HWND hWnd)
  {
    // remember the window handle (HWND)
    mhWnd = hWnd;

    // get the device context (DC)
    mhDC = GetDC( mhWnd );

    // set the pixel format for the DC
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
      PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
    pfd.cDepthBits   = 16;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int format = ChoosePixelFormat( mhDC, &pfd );
    SetPixelFormat( mhDC, format, &pfd );

    // create the render context (RC)
    mhRC = wglCreateContext( mhDC );

    // make it the current render context
    wglMakeCurrent( mhDC, mhRC );
  }

  void purge()
  {
    if ( mhRC )
    {
      wglMakeCurrent( NULL, NULL );
      wglDeleteContext( mhRC );
    }
    if ( mhWnd && mhDC )
    {
      ReleaseDC( mhWnd, mhDC );
    }
    reset();
  }


  HWND mhWnd;
  HDC mhDC;
  HGLRC mhRC;
private:

  void reset()
  {
    mhWnd = NULL;
    mhDC = NULL;
    mhRC = NULL;
  }



}g_GLContext;
//-----------------------------------------------------------------------------
// Name: SetupPixelFormat()
// Desc: yeh, it is
//-----------------------------------------------------------------------------
bool SetupPixelFormat()
{
  PIXELFORMATDESCRIPTOR pfd=
  {
    sizeof(PIXELFORMATDESCRIPTOR),	// Размер этой структуры
    1,				// Номер версии (?)
    PFD_DRAW_TO_WINDOW |// Формат для Окна68
    PFD_SUPPORT_OPENGL |// Формат для OpenGL
    PFD_DOUBLEBUFFER|// Формат для двойного буфера
    PFD_GENERIC_ACCELERATED,
    PFD_TYPE_RGBA,	// Требуется RGBA формат
    32,				// Выбор глубины цвета
    0, 0, 0, 0, 0, 0,// Игнорирование цветовых битов (?)
    0,				// нет буфера прозрачности
    0,				// Сдвиговый бит игнорируется (?)
    0,				// Нет буфера аккумуляции
    0, 0, 0, 0,		// Биты аккумуляции игнорируются (?)
    16,				//  битный Z-буфер (буфер глубины)  
    8,				// буфер траффарета
    0,				// Нет вспомогательных буферов (?)
    PFD_MAIN_PLANE,	// Главный слой рисования
    0,				// Резерв (?)
    0, 0, 0			// Маски слоя игнорируются (?)
  };
  g_hdc = GetDC(g_hWnd);	// Получить контекст устройства для окна
  g_PixelFormat = ChoosePixelFormat(g_hdc, &pfd);
  // Найти ближайшее совпадение для нашего формата пикселов
  if (!g_PixelFormat)
  {
    MessageBox(0,"Can't Find A Suitable PixelFormat.","Error",MB_OK|MB_ICONERROR);
    PostQuitMessage(0);
    return false;
  }

  if(!SetPixelFormat(g_hdc,g_PixelFormat,&pfd))
  {
    MessageBox(0,"Can't Set The PixelFormat.","Error",MB_OK|MB_ICONERROR);
    PostQuitMessage(0);
    return false;
  }
  return true;
}
//-----------------------------------------------------------------------------
// Name: InitRC()
// Desc: GL Render Context Initialization
//-----------------------------------------------------------------------------
bool InitRC()
{
  g_hdc = GetDC(g_hWnd);
  g_GLContext.init(g_hWnd);
  //SetupPixelFormat();
  g_hrc=g_GLContext.mhRC;
  if(!g_hrc)
  {
    MessageBox(0,"Can't Create A GL Rendering Context.","Error",MB_OK|MB_ICONERROR);
    return false;
  }
  if(!wglMakeCurrent(g_hdc, g_hrc))
  {
    MessageBox(0,"Can't activate GLRC.","Error",MB_OK|MB_ICONERROR);
    return false;
  }
  return true;
}

extern "C" bool RenderInit()
{
  InitRC();
  return true;
}

extern "C" void RenderSwapBuffers()
{
  SwapBuffers(g_hdc);
}

extern "C" void RenderRelease()
{
  wglMakeCurrent(g_hdc, NULL);
  if(g_hrc)
    wglDeleteContext(g_hrc);
  g_hrc = NULL;
  if(g_hdc)
    ReleaseDC (g_hWnd, g_hdc);
  g_hdc = NULL;
}

//////////////////////////////////////////////////////////////////////////
// textures

static bool ReadFile(const char* szFileName, std::vector<unsigned char>& data)
{
  data.clear();
  HANDLE hFile = CreateFile(szFileName, GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (INVALID_HANDLE_VALUE == hFile)
    return false;

  DWORD file_size = GetFileSize(hFile, NULL);
  data.resize(file_size);
  ReadFile(hFile, &data[0], (DWORD)data.size(), &file_size, NULL);
  CloseHandle(hFile);
  return data.size() == file_size;
}


static bool CreateTextureFromJng(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  std::vector<unsigned char> jng;
  if(!ReadFile(szFileName, jng))
    return false;

  u32 pngoffset = *((u32*)&jng[4]);
  // decode jpg to RGBA
  int width, height, comp;
  u8* data = stbi_load_from_memory(&jng[8], pngoffset - 8, &width, &height, &comp, STBI_rgb_alpha);
  EASSERT(data);
  if (!data)
    return false;

  EASSERT(width  == (int)nextpow2(width));
  EASSERT(height == (int)nextpow2(height));

  // decode jng to greyscale
  int awidth, aheight;
  u8* adata = stbi_load_from_memory(&jng[pngoffset], jng.size() - pngoffset, &awidth, &aheight, &comp, STBI_grey);
  if (!adata) {
    stbi_image_free(data);
    return false;
  }

  // assemble image
  EASSERT(width  == awidth);
  EASSERT(height == aheight);
  for(int y = 0; height != y; ++y)
    for(int x = 0; width != x; ++x)
      data[4 * y * width + 4 * x + 3] = adata[y * width + x];

  stbi_image_free(adata);

  // Create the texture
  GLenum fmt = GL_RGBA;
  EASSERT(0 != fmt);
  if (0 == fmt) {
    stbi_image_free(data);
    return false;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  if(rw)
    *rw = width;
  if(rh)
    *rh = height;

  if(w)
    *w = width;
  if(h)
    *h = height;

  return true;
}

static bool CreateTextureFromPng(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  std::vector<unsigned char> png;
  if(!ReadFile(szFileName, png))
    return false;

  // decode png
  int width, height, comp;
  u8* data = stbi_load_from_memory(&png[0], png.size(), &width, &height, &comp, STBI_default);
  EASSERT(data);
  if (!data)
    return false;

  EASSERT(width  == (int)nextpow2(width));
  EASSERT(height == (int)nextpow2(height));

  // Create the texture
  GLenum fmt = (STBI_rgb == comp) ? GL_RGB : (STBI_rgb_alpha == comp) ? GL_RGBA : 0;
  EASSERT(0 != fmt);
  if (0 == fmt) {
    stbi_image_free(data);
    return false;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  if(rw)
    *rw = width;
  if(rh)
    *rh = height;

  if(w)
    *w = width;
  if(h)
    *h = height;

  return true;
}

//////////////////////////////////////////////////////////////////////////
// DDS support

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
# define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
# define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
# define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

static GLuint GLFormat(dds::DDSFormat fmt)
{
  switch (fmt)
  {
  case dds::FORMAT_DXT1:
    return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
  case dds::FORMAT_DXT3:
    return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
  case dds::FORMAT_DXT5:
    return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
  }
  return 0;
}

typedef GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level,
                                                           GLenum internalFormat, GLsizei width,
                                                           GLsizei height, GLint border,
                                                           GLsizei imageSize, const GLvoid *data);



static bool CreateTextureFromDXT(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  // read file
  std::vector<unsigned char> rawdata;
  if(!ReadFile(szFileName, rawdata))
    return false;

  const dds::DDS_Header_t& header(*(dds::DDS_Header_t*)&rawdata[0]);

  EASSERT(nextpow2(header.Width ) == header.Width );
  EASSERT(nextpow2(header.Height) == header.Height);

  static PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress("glCompressedTexImage2D");
  GLuint glformat = GLFormat(getFormat(header));
  if(glCompressedTexImage2D && 0 != glformat)
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, glformat, header.Width, header.Height, 0, rawdata.size() - sizeof(dds::DDS_Header_t), &rawdata[sizeof(dds::DDS_Header_t)]);
  else
  {
    // decode texture
    std::vector<unsigned char> image(header.Width * header.Height * 4);

    if(!dds::decodeCompressedImage(&image[0], &rawdata[sizeof(dds::DDS_Header_t)], header))
      return false;

    // Create the texture
    GLenum fmt = GL_RGBA;
    if(0 == fmt)
      return false;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, texid);
    glBindTexture(GL_TEXTURE_2D, *texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, header.Width, header.Height, 0, fmt, GL_UNSIGNED_BYTE, &image[0]);
  }

  if(rw)
    *rw = header.Width;
  if(rh)
    *rh = header.Height;

  if(w)
    *w = header.Width;
  if(h)
    *h = header.Height;

  return true;
}

#undef CreateTextureRes

static bool CreateTextureFromWebP(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  std::vector<unsigned char> png;
  if(!ReadFile(szFileName, png))
    return false;

  // decode png
  int width, height, comp;
  u8* data = webp_load_from_memory(&png[0], png.size(), &width, &height, &comp);
  EASSERT(data);
  if (!data)
    return false;

  EASSERT(width  == (int)nextpow2(width));
  EASSERT(height == (int)nextpow2(height));

  // Create the texture
  GLenum fmt = (3 == comp) ? GL_RGB : (4 == comp) ? GL_RGBA : 0;
  EASSERT(0 != fmt);
  if (0 == fmt) {
    webp_free(data);
    return false;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  webp_free(data);

  if(rw)
    *rw = width;
  if(rh)
    *rh = height;

  if(w)
    *w = width;
  if(h)
    *h = height;

  return true;
}


bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

extern "C" bool CreateTextureRes(const char* name, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  if(w)  *w  = 0;
  if(h)  *h  = 0;
  if(rw) *rw = 0;
  if(rh) *rh = 0;

  std::string s(name);

  replace(s, ".png", ".webp");
  if(CreateTextureFromWebP(s.c_str(), texid, w, h, rw, rh))
    return true;

  replace(s, ".webp", ".jng");
  if(CreateTextureFromJng(s.c_str(), texid, w, h, rw, rh))
    return true;

  replace(s, ".jng", ".dds");
  if(CreateTextureFromDXT(s.c_str(), texid, w, h, rw, rh))
    return false;

  if(NULL != strstr(name,".png") && !CreateTextureFromPng(name, texid, w, h, rw, rh))
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////////
// Other stuff (not render-related)

extern "C" float GetTimeInSecSinceCPUStart()
{
  static LARGE_INTEGER freq = {0};
  if(0 == freq.QuadPart)
    QueryPerformanceFrequency(&freq);

  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);
  return time.QuadPart / (f32) freq.QuadPart;
}

extern "C" u64 GetCPUCycles()
{
  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);
  return time.QuadPart;
}

extern "C" u64 GetCPUCyclesPerMSec()
{
  static u64 cycles = 0;
  if(0 != cycles)
    return cycles;
  LARGE_INTEGER freq = {0};
  QueryPerformanceFrequency(&freq);
  cycles = freq.QuadPart / 1000;
  return cycles;
}

extern "C" void OpenURL(const char* url)
{
  ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
