#ifndef K_CALLBACKS_H 
#define K_CALLBACKS_H 

#  ifdef __cplusplus
extern "C" {
#  endif


enum eHwKey {
  hwkBack       = 0,
  hwkMenu       = 1,
  hwkCount
};

// ----- callbacks

// (called by engine)
extern bool game_init();
extern void game_tick();
extern void game_render();
extern void game_release();
extern void game_pause(bool pause);

// (called by platform)
extern void input_touch_begin (int x, int y, unsigned int id);
extern void input_touch_move  (int x, int y, unsigned int id);
extern void input_touch_end   (int x, int y, unsigned int id);
extern void input_touch_cancel(int x, int y, unsigned int id);
extern void input_hw_key_down (enum eHwKey key);

extern void input_acceleration(float x, float y, float z);

extern void screen_resize(unsigned int w, unsigned int h);

// internal

extern float GetTimeInSecSinceCPUStart();
extern unsigned long long GetCPUCycles();
extern unsigned long long GetCPUCyclesPerMSec(); // per millisecond

extern void OpenURL(const char* url);
extern void TextBox(const wchar_t * const szTitle, wchar_t* buffer, unsigned int size);
extern void OnTextBoxResult(bool res);
extern bool GetLocale(char[2]);

#  ifdef __cplusplus
}
#  endif

#ifdef _WIN32
#  define WINDOWS_PLATFORM
#elif __APPLE__
#  include "TargetConditionals.h"
#  if TARGET_OS_IPHONE
#    define IPHONE_PLATFORM
#  elif TARGET_OS_MAC
#    define MACOS_PLATFORM
#  else
//   Unsupported apple platform
#  endif
#else
// Unsupported platform
#endif

// ----- strcmp
#ifdef __APPLE__
# define stricmp strcasecmp
#endif

#ifdef ANDROID_PLATFORM
# define stricmp strcasecmp
#endif

#ifdef WIN32
# define stricmp _stricmp
#endif

// ----- asserts
#ifdef WIN32
extern bool AssertFun(bool& enabled, const char* FileName, int LineNum, const char* TextCondition, ...);
#  define EASSERT(exp, ...)  \
{ \
  static bool enable = true; \
  if(enable) \
    if(!(exp) && AssertFun(enable, __FILE__, __LINE__, #exp, __VA_ARGS__)) \
      __debugbreak(); \
}
#  define EALWAYS_ASSERT(MSG)  \
{ \
  static bool enable = true; \
  if(enable) \
    if(AssertFun(enable, __FILE__, __LINE__, MSG)) \
      __debugbreak(); \
}
#else // WIN32
#  define EASSERT(X, ...)      {}
#  define EALWAYS_ASSERT(MSG)  {}
#endif // WIN32





#endif // K_CALLBACKS_H 
