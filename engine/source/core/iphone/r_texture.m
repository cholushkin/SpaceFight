#import <QuartzCore/QuartzCore.h>

#if TARGET_OS_IPHONE
# import <UIKit/UIKit.h>
# import <OpenGLES/EAGL.h>
# import <OpenGLES/ES1/gl.h>
# import <OpenGLES/ES1/glext.h>
# import <OpenGLES/EAGLDrawable.h>
# import "gzip/NSData+gzip.h"
#else
# import <AppKit/AppKit.h>
# import <OpenGL/OpenGL.h>
# import <OpenGL/gl.h>
#endif

#include "../loaders/stb_image.h"
#include "../loaders/r_tex_pvr.h"
#include "../loaders/r_tex_webp.h"

static inline unsigned int nextpow2(unsigned int val)
{
  --val;
  val = (val >>  1) | val;
  val = (val >>  2) | val;
  val = (val >>  4) | val;
  val = (val >>  8) | val;
  val = (val >> 16) | val;
  ++val;
  return val;
}

bool RenderInit()
{
  return true;
}

void RenderRelease()
{
}

void RenderSwapBuffers()
{
}

#define REPORT_DIMENSIONS(rw, rh, w, h, width, height) \
  if(rw) *rw = width; \
  if(rh) *rh = height;\
  if(w)  *w  = width; \
  if(h)  *h  = height;

#if TARGET_OS_IPHONE

bool LoadPvrTexture(NSData* data, GLuint *texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  struct PVRTextureHeaderV3* header = (struct PVRTextureHeaderV3 *)[data bytes];
  uint32_t pvrTag = CFSwapInt32LittleToHost(header->u32Version);

  if (gPVRTexIdentifier != pvrTag)
    return false;

  uint32_t format = header->u64PixelFormat;

  if (ePVRTPF_PVRTCI_2bpp_RGBA != format &&
      ePVRTPF_PVRTCI_4bpp_RGBA != format)
    return false;

  uint32_t internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;

  if (ePVRTPF_PVRTCI_4bpp_RGBA == format)
    internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
  else if (ePVRTPF_PVRTCI_2bpp_RGBA == format)
    internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;

  uint32_t width      = CFSwapInt32LittleToHost(header->u32Width);
  uint32_t height     = CFSwapInt32LittleToHost(header->u32Height);
  //bool     hasAlpha   = CFSwapInt32LittleToHost(header->bitmaskAlpha);
  //uint32_t dataLength = CFSwapInt32LittleToHost(header->dataLength);

  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // load only 1st mipmap level
  glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, [data length] - PVRTEX3_HEADERSIZE, ((uint8_t*)[data bytes]) + PVRTEX3_HEADERSIZE);

  REPORT_DIMENSIONS(rw, rh, w, h, width, height);

  return true;
}

bool CreatePvrTextureRes(NSString* path, GLuint *texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSData *data = [[NSData alloc] initWithContentsOfFile:path];
  bool res = LoadPvrTexture(data, texid, w, h, rw, rh);
  return res;
}

bool CreatePvrGZTextureRes(NSString* path, GLuint *texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSData *data = [[NSData alloc] initWithContentsOfFile:path];
  NSData* defl = [data gzipInflate];
  data = nil;
  if(!defl)
    return false;
  return LoadPvrTexture(defl, texid, w, h, rw, rh);
}

#endif // TARGET_OS_IPHONE

static bool CreateTextureFromPng(NSString* path, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSData *nsdata = [[NSData alloc] initWithContentsOfFile:path];
  if(!nsdata)
    return false;
  const char* png = [nsdata bytes];
  int sizeOfFile  = [nsdata length];
  if(!png)
    return false;

  // decode png
  int width, height, comp;
  char* data = (char*)stbi_load_from_memory((stbi_uc const*)png, sizeOfFile, &width, &height, &comp, STBI_default);
  nsdata = nil;
  if (!data)
    return false;

  // Create the texture
  GLenum fmt = (STBI_rgb == comp) ? GL_RGB : (STBI_rgb_alpha == comp) ? GL_RGBA : 0;
  if (0 == fmt) {
    stbi_image_free(data);
    return false;
  }

  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D (GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  REPORT_DIMENSIONS(rw, rh, w, h, width, height);

  return true;
}

static bool CreateTextureFromWebP(NSString* path, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSData *nsdata = [[NSData alloc] initWithContentsOfFile:path];
  if(!nsdata)
    return false;
  const char* png = [nsdata bytes];
  int sizeOfFile  = [nsdata length];
  if(!png)
    return false;

  // decode png
  int width, height, comp;
  char* data = (char*)webp_load_from_memory(png, sizeOfFile, &width, &height, &comp);
  nsdata = nil;
  if (!data)
    return false;

  // Create the texture
  GLenum fmt = (3 == comp) ? GL_RGB : (4 == comp) ? GL_RGBA : 0;
  if (0 == fmt) {
    webp_free(data);
    return false;
  }

  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D (GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  webp_free(data);

  REPORT_DIMENSIONS(rw, rh, w, h, width, height);

  return true;
}


static bool CreateTextureFromJng(NSString* path, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSData *nsdata = [[NSData alloc] initWithContentsOfFile:path];
  if(!nsdata)
    return false;
  const char* jng = [nsdata bytes];
  int sizeOfFile  = [nsdata length];
  if(!jng)
    return false;

  UInt32 pngoffset = *((UInt32*)&jng[4]);
  // decode jpg to RGBA
  int width, height, comp;
  char* data = stbi_load_from_memory(&jng[8], pngoffset - 8, &width, &height, &comp, STBI_rgb_alpha);
  //EASSERT(data);
  if (!data){
    nsdata = nil;
    return false;
  }

  //EASSERT(width  == (int)nextpow2(width));
  //EASSERT(height == (int)nextpow2(height));

  // decode jng to greyscale
  int awidth, aheight;
  char* adata = stbi_load_from_memory(&jng[pngoffset], sizeOfFile - pngoffset, &awidth, &aheight, &comp, STBI_grey);
  nsdata = nil;
  if (!adata) {
    stbi_image_free(data);
    return false;
  }

  // assemble image
  //EASSERT(width  == awidth);
  //EASSERT(height == aheight);
  for(int y = 0; height != y; ++y)
    for(int x = 0; width != x; ++x)
      data[4 * y * width + 4 * x + 3] = adata[y * width + x];

  stbi_image_free(adata);

  // Create the texture
  GLenum fmt = GL_RGBA;
  //EASSERT(0 != fmt);
  if (0 == fmt) {
    stbi_image_free(data);
    return false;
  }

  glGenTextures(1, texid);
  glBindTexture(GL_TEXTURE_2D, *texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  REPORT_DIMENSIONS(rw, rh, w, h, width, height);

  return true;
}

bool CreateTextureRes(const char* szFileName, GLuint *texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  NSString* fullpath    = [[NSString alloc] initWithCString: szFileName encoding:NSASCIIStringEncoding];
  NSString *nsFilename  = [fullpath stringByDeletingPathExtension];

  NSString* path;

#if TARGET_OS_IPHONE
  // try zipped PVR
  path = [[NSBundle mainBundle] pathForResource:nsFilename ofType:@"gz" inDirectory:@"res"];
  if(path)
    return CreatePvrGZTextureRes(path, texid, w, h, rw, rh);

  // try PVR
  path = [[NSBundle mainBundle] pathForResource:nsFilename ofType:@"pvr" inDirectory:@"res"];
  if(path)
    return CreatePvrTextureRes(path, texid, w, h, rw, rh);

#endif // TARGET_OS_IPHONE
  // try jng
  path = [[NSBundle mainBundle] pathForResource:nsFilename ofType:@"jng" inDirectory:@"res"];
  if(path)
    return CreateTextureFromJng(path, texid, w, h, rw, rh);

  path = [[NSBundle mainBundle] pathForResource:nsFilename ofType:@"webp" inDirectory:@"res"];
  if(path)
    return CreateTextureFromWebP(path, texid, w, h, rw, rh);

  // try default
  path = [[NSBundle mainBundle] pathForResource:nsFilename ofType:[fullpath pathExtension] inDirectory:@"res"];
  return CreateTextureFromPng(path, texid, w, h, rw, rh);
}
