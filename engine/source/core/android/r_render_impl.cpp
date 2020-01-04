#include "core/common/com_misc.h"
#include "../r_device_gl.h"
#include "../loaders/stb_image.h"
#include "../loaders/r_tex_webp.h"

#include <android/log.h>
#include <jni.h>
#include "jni-stuff.h"

#define  LOG_TAG    "render"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__) 

#include <map>
#include <string>

extern JNIEnv* env;
extern const char * const sCLASS_NAME;

extern "C" bool RenderInit() { return true; }
extern "C" void RenderSwapBuffers() {}
extern "C" void RenderRelease() {}

static std::map<GLuint, std::string> g_textures;

extern char* getFileContent(const char* pszPath, u32& sizeOfFile);


static bool CreateTextureFromJng(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  u32 sizeOfFile(0);
  char* jng = getFileContent(szFileName, sizeOfFile);
  if(!jng)
    return false;

  u32 pngoffset = *((u32*)&jng[4]);
  // decode jpg to RGBA
  int width, height, comp;
  u8* data = stbi_load_from_memory((stbi_uc const*)&jng[8], pngoffset - 8, &width, &height, &comp, STBI_rgb_alpha);
  EASSERT(data);
  if (!data) {
    SAFE_DELETE_ARRAY(jng);
    return false;
  }

  // decode jng to greyscale
  int awidth, aheight;
  u8* adata = stbi_load_from_memory((stbi_uc const*)&jng[pngoffset], sizeOfFile - pngoffset, &awidth, &aheight, &comp, STBI_grey);
  SAFE_DELETE_ARRAY(jng);
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

// todo: merge me with win32/iPhone versions
static bool CreateTextureFromPng(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  u32 sizeOfFile(0);
  char* png = getFileContent(szFileName, sizeOfFile);
  if(!png)
    return false;

  // decode png
  int width, height, comp;
  u8* data = (u8*)stbi_load_from_memory((stbi_uc const*)png, sizeOfFile, &width, &height, &comp, STBI_default);
  SAFE_DELETE_ARRAY(png);
  EASSERT(data);
  if (!data)
    return false;

  EASSERT(width  == nextpow2(width));
  EASSERT(height == nextpow2(height));

  // Create the texture
  GLenum fmt = (STBI_rgb == comp) ? GL_RGB : (STBI_rgb_alpha == comp) ? GL_RGBA : 0;
  EASSERT(0 != fmt);
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

static bool CreateTextureFromWebP(const char* szFileName, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  u32 sizeOfFile(0);
  char* png = getFileContent(szFileName, sizeOfFile);
  if(!png)
    return false;

  int width, height, comp;
  u8* data = webp_load_from_memory((unsigned char*)&png[0], sizeOfFile, &width, &height, &comp);
  SAFE_DELETE_ARRAY(png);
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

extern "C" bool CreateTextureRes(const char* name, GLuint* texid, unsigned int* w, unsigned int* h, unsigned int* rw, unsigned int* rh)
{
  if((NULL == name) || (NULL == env))
    return false;

  ClassRef cls(*env,env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "resolveTextureName", "(Ljava/lang/String;)Ljava/lang/String;");
  if(NULL == m)
    return false;

  StrRef  jname(*env, env->NewStringUTF(name));
  JString tname(*env, (jstring)env->CallStaticObjectMethod(cls, m, (jstring)jname));
  
  if(NULL != strstr(tname.c_str(),".webp")) {
    if(!CreateTextureFromWebP(tname.c_str(), texid, w, h, rw, rh))
      return false;
  }
  else if(NULL != strstr(tname.c_str(),".jng")) {
    if(!CreateTextureFromJng(tname.c_str(), texid, w, h, rw, rh))
      return false;
  }
  else if(NULL != strstr(tname.c_str(),".png")) {
    if(!CreateTextureFromPng(tname.c_str(), texid, w, h, rw, rh))
      return false;
  }

  g_textures[*texid] = std::string(name);
  return true;
}

IMPLEMENT_NODE(r::TextureImpl);

extern "C" int OnRestoreTextures() // todo: return bool or void
{
  // store original IDs
  std::map<GLuint, std::string> textures;
  std::swap(g_textures, textures);

  // destroy old textures
  for(std::map<GLuint, std::string>::const_iterator it = textures.begin(); textures.end() != it; ++it)
    glDeleteTextures( 1, &(*it).first );

  // recreate textures using same name
  r::TextureImpl* ti = alg::INode<r::TextureImpl>::first();
  while(ti)
  {
    // ugly
    GLuint glID = ti->getId();
    std::map<GLuint, std::string>::const_iterator it = textures.find(glID);
    if(textures.end() != it)
      if(CreateTextureRes((*it).second.c_str(), &glID, NULL, NULL, NULL, NULL))
        ti->restore(glID);
    ti = ti->next();
  }
  return 1;
}
