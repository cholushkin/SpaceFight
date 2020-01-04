#include "core/render/r_render.h" // screen sizes there
#include "platform.h"
#include "time.h"
#include "jni-stuff.h"

extern JNIEnv* env;
extern const char * const sCLASS_NAME = "com/damn/engine/Platform";

char* getFileContent(const char* pszPath, u32& sizeOfFile)
{
  if(NULL == pszPath)
    return NULL;

  //LOGI("getFileContent %s", pszPath);
  if(NULL == env){
    //LOGE("no env in AutoFile");
    return NULL;
  }

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "getData", "(Ljava/lang/String;)[B");
  if(NULL == m)
    return NULL;

  StrRef path(*env, env->NewStringUTF(pszPath));
  ArrBRef arr(*env, (jbyteArray)env->CallStaticObjectMethod(cls, m, (jstring)path));
  if(!arr)
    return NULL;

  jsize size = env->GetArrayLength(arr);
  char* vec = new char[size];
  env->GetByteArrayRegion(arr, 0, size, (jbyte*)&vec[0]);
  sizeOfFile = size;
  return vec;
}

extern "C" FILE* open_file(const char* pszPath, bool bRead, bool bIsResourceFile)
{
  // assert(!bIsResourceFile || bRead)
  if(NULL == pszPath)
    return NULL;

  //LOGI("open_file %s", pszPath);
  if(NULL == env){
    //LOGE("no env in AutoFile");
    return NULL;
  }

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "getUserDataPath", "(Ljava/lang/String;)Ljava/lang/String;");
  if(NULL == m)
    return NULL;

  StrRef path(*env, env->NewStringUTF(pszPath));
  JString str(*env, (jstring)env->CallStaticObjectMethod(cls, m, (jstring)path));
  if(!str)
    return NULL;
  return fopen(str.c_str(), bRead ? "rb" : "wb");
}

extern "C" void TextBox(const wchar_t * const szTitle, wchar_t* buffer, unsigned int size)
{
  buffer[0] = L'\0';
  OnTextBoxResult(true);
}

extern "C" bool GetLocale(char loc[2])
{
  static char locale[3] = {0};
  if(!locale[0])
  {
    if(NULL == env) {
      //LOGE("no env in AutoFile");
      return false;
    }

    ClassRef cls(*env, env->FindClass(sCLASS_NAME));
    jmethodID m = env->GetStaticMethodID(cls, "getLocale", "()Ljava/lang/String;");
    if(NULL == m)
      return false;

    JString str(*env, (jstring)env->CallStaticObjectMethod(cls, m));
    if(!str)
      return false;

    const char* sys_loc = str.c_str();
    locale[0] = sys_loc[0];
    locale[1] = sys_loc[1];
  }
  loc[0] = locale[0];
  loc[1] = locale[1];
  return true;
}

extern "C" void OpenURL(const char* url) {
  if(NULL == url)
    return;

  if(NULL == env) {
    //LOGE("no env in AutoFile");
    return;
  }

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "openURL", "(Ljava/lang/String;)V");
  if(NULL == m)
    return;

  StrRef jurl(*env, env->NewStringUTF(url));
  env->CallStaticVoidMethod(cls, m, (jstring)jurl);
}

extern "C" float GetTimeInSecSinceCPUStart()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  static u64 base = now.tv_sec;
  return (float)(now.tv_sec - base) + now.tv_nsec / 1000000000.0;
}

extern "C" u64 GetCPUCycles()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  static u64 base = now.tv_sec;
  return (now.tv_sec - base) * (1000000000 / 1000) + now.tv_nsec / 1000;
}

extern "C" u64 GetCPUCyclesPerMSec()
{
  return 1000;
}
