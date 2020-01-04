#ifndef simplelog_h__
#define simplelog_h__
#include "core/common/com_misc.h"
#include "core/io/io_base.h"

NAMESPACE_BEGIN(dlog)

class ILogger;

class Log
{
public:
  enum ePriorityConstants
  {
    PRIORITY_VERBOSE  = 1 << 1,
    PRIORITY_DEBUG    = 1 << 2,
    PRIORITY_INFO     = 1 << 3,
    PRIORITY_WARNING  = 1 << 4,
    PRIORITY_ERROR    = 1 << 5,
    PRIORITY_ASSERT   = 1 << 6,
    LEVEL_NOTHING     = 1 << 7
  };

  ~Log();
  static Log*   GetInstance();
  void          EnableLogger ( u8 channel, ILogger* logger ); // owns logger
  void          DisableLogger( u8 channel );
  ILogger*      GetLogger    ( u8 channel );

  void PrintLog( 
    bool tag_autocolor_switch, 
    const char* subsystem, 
    Log::ePriorityConstants priority, 
    const char* i_szFile, 
    int i_iLine, 
    bool endl,
    const wchar_t* msg );

  void PrintLog( 
    bool tag_autocolor_switch, 
    const char* subsystem, 
    Log::ePriorityConstants priority, 
    const char* i_szFile, 
    int i_iLine, 
    bool endl,
    const char* msg );

private:
  Log();
  friend class ILogger;
  static const u16 MAX_CHANNELS = 16;  
  static u32    s_msgUid;
  ILogger*      m_loggers[MAX_CHANNELS];  
};

class ILogger 
{
public:
  ILogger();
  virtual ~ILogger(){}
  void SetLevelFilter(Log::ePriorityConstants level);
  // void SetSubsystemFilter // todo: implement me
  void SetMessageStructureFormat(const char* format);
  /* // todo: support me
  f - file
  l - line
  i - message number
  s - subsystem
  p - priority
  t - time
  m - message
  */


  void Write( 
    const char* msg,
    const char* subsystem, 
    Log::ePriorityConstants priority, 
    const char* i_szFile, 
    int i_iLine, 
    bool tag_autocolor_switch, 
    bool endl);

  void Write( 
    const wchar_t* msg,
    const char* subsystem, 
    Log::ePriorityConstants priority, 
    const char* i_szFile, 
    int i_iLine, 
    bool tag_autocolor_switch, 
    bool endl);

protected:
  virtual void Output      ( const wchar_t* logStr ) = 0;
  virtual void OutputNoTags( const wchar_t* logStr, u32 color ) = 0;
  virtual void Output      ( const char* logStr ) = 0;
  virtual void OutputNoTags( const char* logStr, u32 color ) = 0;
  virtual bool FilterPass(Log::ePriorityConstants priority);
private:
  u8 m_levelFilter;
  const char* m_msgStructureFormat;
};

//////////////////////////////////////////////////////////////////////////
// LoggerFile


#define NLOGV( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_VERBOSE,  __FILE__, __LINE__, true, STR )
#define NLOGD( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_DEBUG,    __FILE__, __LINE__, true, STR )
#define NLOGI( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_INFO,     __FILE__, __LINE__, true, STR )
#define NLOGW( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_WARNING,  __FILE__, __LINE__, true, STR )
#define NLOGE( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_ERROR,    __FILE__, __LINE__, true, STR )
#define NLOGA( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_ASSERT,   __FILE__, __LINE__, true, STR )

#define LOGV( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_VERBOSE,  __FILE__, __LINE__, false, STR )
#define LOGD( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_DEBUG,    __FILE__, __LINE__, false, STR )
#define LOGI( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_INFO,     __FILE__, __LINE__, false, STR )
#define LOGW( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_WARNING,  __FILE__, __LINE__, false, STR )
#define LOGE( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_ERROR,    __FILE__, __LINE__, false, STR )
#define LOGA( SUBSYSTEM, STR )        dlog::Log::GetInstance()->PrintLog( true, SUBSYSTEM, dlog::Log::PRIORITY_ASSERT,   __FILE__, __LINE__, false, STR )

#define NLOGV_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_VERBOSE,   __FILE__, __LINE__, true, STR )
#define NLOGD_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_DEBUG,     __FILE__, __LINE__, true, STR )
#define NLOGI_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_INFO,      __FILE__, __LINE__, true, STR )
#define NLOGW_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_WARNING,   __FILE__, __LINE__, true, STR )
#define NLOGE_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_ERROR,     __FILE__, __LINE__, true, STR )
#define NLOGA_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_ASSERT,    __FILE__, __LINE__, true, STR )

#define LOGV_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_VERBOSE,   __FILE__, __LINE__, false, STR )
#define LOGD_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_DEBUG,     __FILE__, __LINE__, false, STR )
#define LOGI_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_INFO,      __FILE__, __LINE__, false, STR )
#define LOGW_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_WARNING,   __FILE__, __LINE__, false, STR )
#define LOGE_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_ERROR,     __FILE__, __LINE__, false, STR )
#define LOGA_COLOR( SUBSYSTEM, STR )  dlog::Log::GetInstance()->PrintLog( false, SUBSYSTEM, dlog::Log::PRIORITY_ASSERT,    __FILE__, __LINE__, false, STR )


NAMESPACE_END(dlog)
#endif // simplelog_h__