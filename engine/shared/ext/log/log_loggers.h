#include "core/log/log_log.h"

#ifdef WIN32
#include "windows.h"
#endif

#ifndef log_loggers_h__
#define log_loggers_h__

NAMESPACE_BEGIN(dlog)

// ----------------------------------------------------------------------------
class LoggerNull : public ILogger
{
public:
  LoggerNull() {}
  virtual void Output( const wchar_t* /*logStr*/ ){}
  virtual void OutputNoTags( const wchar_t* /*logStr*/, u32 /*color*/){}
  virtual void Output      ( const char* /*logStr*/ ){}
  virtual void OutputNoTags( const char* /*logStr*/, u32 /*color*/ ){}
};


// ----------------------------------------------------------------------------
class LoggerFile : public ILogger
{
public:
  LoggerFile(const char* fname);
protected:
  virtual void Output( const wchar_t* logStr );
  virtual void OutputNoTags( const wchar_t* logStr, u32 color );
  virtual void Output( const char* logStr );
  virtual void OutputNoTags( const char* logStr, u32 color );
private:
  io::FileStream m_file;
};


// ----------------------------------------------------------------------------
class LoggerGameConsole : public ILogger
{
public:
  LoggerGameConsole() {}
  virtual void Output( const wchar_t* /*logStr*/ ){};
  virtual void OutputNoTags( const wchar_t* /*logStr*/, u32 /*color*/){};
};


// ----------------------------------------------------------------------------
#ifdef WIN32
class LoggerSysConsole : public ILogger
{
  HANDLE m_hSystemConsole;
  WORD m_defaultAttribs;
  u8 Color2Index(u32 clr) const;
public:
  LoggerSysConsole(const char* cname, u16 width, u16 height); 
  ~LoggerSysConsole();
  virtual void Output( const wchar_t* /*logStr*/ );
  virtual void OutputNoTags( const wchar_t* /*logStr*/, u32 /*color*/);
  virtual void Output( const char* /*logStr*/ );
  virtual void OutputNoTags( const char* /*logStr*/, u32 /*color*/);
};
#else
class LoggerSysConsole : public LoggerNull
{
public:
  LoggerSysConsole(const char* cname, u16 width, u16 height); 
  virtual void Output( const char* /*logStr*/ );
  virtual void OutputNoTags( const char* /*logStr*/, u32 /*color*/);
};
#endif


// ----------------------------------------------------------------------------
class LoggerVSOutputWindow : public ILogger
{
public:
  LoggerVSOutputWindow() {}
  virtual void Output( const wchar_t* /*logStr*/ ){};
  virtual void OutputNoTags( const wchar_t* /*logStr*/, u32 /*color*/){};
};


NAMESPACE_END(dlog)
#endif // log_loggers_h__
