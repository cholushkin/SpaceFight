#include "core/log/log_log.h"
#include "platform.h"
#include "ext/strings/str_base.h"
#include "ext/math/mt_colors.h"
#include "ext/strings/str_base.h"
#include "ext/strings/str_stringbuilder.h"




NAMESPACE_BEGIN(dlog)
using namespace mt;
using namespace str;


// ----- Log
// ----------------------------------------------------------------------------

u32 Log::s_msgUid = 0;

Log::Log()
{
  for(u8 i=0;i<MAX_CHANNELS;++i)
    m_loggers[i] = NULL;
}

Log::~Log()
{
  for(u8 i=0;i<MAX_CHANNELS;++i)
    DisableLogger(i);
}

Log* Log::GetInstance()
{
  static Log s_instance;
  return &s_instance;
}

void Log::EnableLogger( u8 channel, ILogger* logger )
{
  EASSERT(logger);
  DisableLogger(channel);
  m_loggers[channel] = logger;
}

void Log::DisableLogger( u8 channel )
{
  EASSERT(channel<MAX_CHANNELS);
  SAFE_DELETE (m_loggers[channel] );
}

ILogger* Log::GetLogger( u8 channel )
{
  EASSERT(channel<MAX_CHANNELS);
  return m_loggers[channel];
}

void Log::PrintLog( 
  bool tag_autocolor_switch, 
  const char* subsystem, 
  Log::ePriorityConstants priority, 
  const char* i_szFile, 
  int i_iLine, 
  bool endl,
  const char* msg ) 
{
  ++s_msgUid;
  for(u8 i=0;i<MAX_CHANNELS;++i)
    if(m_loggers[i])
      m_loggers[i]->Write(msg,subsystem,priority,i_szFile,i_iLine,tag_autocolor_switch,endl);
}

void Log::PrintLog( 
  bool tag_autocolor_switch, 
  const char* subsystem, 
  Log::ePriorityConstants priority, 
  const char* i_szFile, 
  int i_iLine, 
  bool endl,
  const wchar_t* msg ) 
{
  ++s_msgUid;
  for(u8 i=0;i<MAX_CHANNELS;++i)
    if(m_loggers[i])
      m_loggers[i]->Write(msg,subsystem,priority,i_szFile,i_iLine,tag_autocolor_switch,endl);
}






// ----- helpers
// ----------------------------------------------------------------------------

inline std::wstring Prior2Str(Log::ePriorityConstants priority)
{
  if(Log::PRIORITY_VERBOSE      == priority) return L"v";
  else if(Log::PRIORITY_DEBUG   == priority) return L"d"; 
  else if(Log::PRIORITY_INFO    == priority) return L"i"; 
  else if(Log::PRIORITY_WARNING == priority) return L"w"; 
  else if(Log::PRIORITY_ERROR   == priority) return L"e"; 
  else if(Log::PRIORITY_ASSERT  == priority) return L"a"; 
  return L"(?)";
}

// $$$ fixme later
//inline std::string ExtractFilename( const std::string& path )
//{
//  return path.substr( path.find_last_of( '\\' ) + 1 );
//}



// ----- ILogger
// ----------------------------------------------------------------------------
ILogger::ILogger()
:  m_levelFilter(255)
,  m_msgStructureFormat( NULL )
{
}

void ILogger::SetLevelFilter(Log::ePriorityConstants level)
{
  m_levelFilter = 0;
  switch (level)
  {
  case Log::LEVEL_NOTHING:
    m_levelFilter = Log::LEVEL_NOTHING;
    break;
  // can be replaced with for(;;)
  case Log::PRIORITY_VERBOSE:
    m_levelFilter |= Log::PRIORITY_VERBOSE;
  case Log::PRIORITY_DEBUG:
    m_levelFilter |= Log::PRIORITY_DEBUG;
  case Log::PRIORITY_INFO:
    m_levelFilter |= Log::PRIORITY_INFO;
  case Log::PRIORITY_WARNING:
    m_levelFilter |= Log::PRIORITY_WARNING;
  case Log::PRIORITY_ERROR:
    m_levelFilter |= Log::PRIORITY_ERROR;
  case Log::PRIORITY_ASSERT:
    m_levelFilter |= Log::PRIORITY_ASSERT;
  }
}

void ILogger::SetMessageStructureFormat(const char* format)
{
  m_msgStructureFormat = format;
}


void ILogger::Write( 
  const wchar_t* msg,
  const char* subsystem, 
  Log::ePriorityConstants priority, 
  const char* i_szFile, 
  int i_iLine, 
  bool tag_autocolor_switch, 
  bool endl)
{
  if(!FilterPass(priority))
    return;

  StringBuilderW sb;
  if(m_msgStructureFormat==NULL)
  {
    static const wchar_t* endlFormat = L"%0\n";
    static const wchar_t* notEndlFormat = L"%0";
    sb(endl?endlFormat:notEndlFormat,msg);
  }
  else
  {
    subsystem;
    i_szFile;
    i_iLine;
    //sb(L"[%0:%1:", Log::s_msgUid, subsystem)(L"%0:%1:%2", Prior2Str(priority).c_str(), i_szFile, i_iLine)(L"]%0",logStr);
    ;
  }

  if(tag_autocolor_switch)      
  {
    // select color 
    u32 clr = COLOR_WHITE;
    if(Log::PRIORITY_VERBOSE       == priority)  clr = COLOR_WHEAT;
    else if (Log::PRIORITY_DEBUG   == priority)  clr = COLOR_GREEN;
    else if (Log::PRIORITY_INFO    == priority)  clr = COLOR_YELLOW;
    else if (Log::PRIORITY_WARNING == priority)  clr = COLOR_MAGENTA;
    else if (Log::PRIORITY_ERROR   == priority)  clr = COLOR_RED;
    else if (Log::PRIORITY_ASSERT  == priority)  clr = COLOR_RED;
    OutputNoTags(sb,clr);
  }
  else
    Output(sb);
}


void ILogger::Write( 
                    const char* msg,
                    const char* subsystem, 
                    Log::ePriorityConstants priority, 
                    const char* i_szFile, 
                    int i_iLine, 
                    bool tag_autocolor_switch, 
                    bool endl)
{
  if(!FilterPass(priority))
    return;

  StringBuilderA sb;
  if(m_msgStructureFormat==NULL)
  {
    static const char* endlFormat = "%0\n";
    static const char* notEndlFormat = "%0";
    sb(endl?endlFormat:notEndlFormat,msg);
  }
  else
  {
    subsystem;
    i_szFile;
    i_iLine;
    //sb(L"[%0:%1:", Log::s_msgUid, subsystem)(L"%0:%1:%2", Prior2Str(priority).c_str(), i_szFile, i_iLine)(L"]%0",logStr);
    ;
  }

  if(tag_autocolor_switch)      
  {
    // select color 
    u32 clr = COLOR_WHITE;
    if(Log::PRIORITY_VERBOSE       == priority)  clr = COLOR_WHEAT;
    else if (Log::PRIORITY_DEBUG   == priority)  clr = COLOR_GREEN;
    else if (Log::PRIORITY_INFO    == priority)  clr = COLOR_YELLOW;
    else if (Log::PRIORITY_WARNING == priority)  clr = COLOR_MAGENTA;
    else if (Log::PRIORITY_ERROR   == priority)  clr = COLOR_RED;
    else if (Log::PRIORITY_ASSERT  == priority)  clr = COLOR_RED;
    OutputNoTags(sb,clr);
  }
  else
    Output(sb);
}

bool ILogger::FilterPass(Log::ePriorityConstants priority )
{
  return 0 != (priority & m_levelFilter);
}


NAMESPACE_END(dlog)
