#include "ext/log/log_loggers.h"
#include "ext/math/mt_colors.h"
#include "ext/primitives/r_font.h"
#include "ext/strings/str_conv.h"
#include "ext/strings/str_base.h"

#ifdef ANDROID_PLATFORM
# include <android/log.h>
# define  console_print_i(...)  __android_log_print(ANDROID_LOG_INFO, "", __VA_ARGS__)
# define  console_print_e(...)  __android_log_print(ANDROID_LOG_ERROR,"", __VA_ARGS__)
#else
# define  console_print_i(...)  printf(__VA_ARGS__)
# define  console_print_e(...)  printf(__VA_ARGS__)
#endif

NAMESPACE_BEGIN(dlog)

#ifdef WIN32

// ----- LoggerFile
// ----------------------------------------------------------------------------

/*
  [c:rrggbb]    - color of font (hex)
  [a:aa]        - alpha (hex)
  [h:n]         - size of font
  [b:n]         - bold
  [i:n]         - italic
  [u:n]         - underlined
  [s:n]         - stroke
  n - parameter
*/


class TagProcessor
{
public:  
  TagProcessor();
  enum eTextStyle {
    tsNormal,
    tsBold,
    tsItalic,
    tsGlow,
    tsEffect1,
    tsEffect2,
    tsEffect3,
  };

  struct TextState{
    u32       m_clr;
    f32       m_h;
    eTextStyle    m_style;
    bool      m_isUnderlined;
    bool      m_isStroke;
  };  

  u32   ParseTag( const wchar_t* str ); // return tag size to skip
  void  ClearState();
  TextState& GetState();
private:
  TextState m_state;
};


TagProcessor::TagProcessor()
{
  ClearState();
}

u32 TagProcessor::ParseTag( const wchar_t* str )
{
  if(str[0] != L'[') 
    return 0; // no open tag error

  // ----- get tag and param
  wchar_t tag=L'a';
  static const u16 maxTagLen = 16;
  char param[maxTagLen];
  u8 ti = 0; // tag cnt
  u8 pi = 0; // param cnt
  u32 p;
  {    
    bool isInParam = false;

    for(p = 0 ;NULL != str[p]; ++p)
    {
      if(p==maxTagLen-1)
        return 0; // too big tag (param)
      if(str[p]==L'[')
        continue;
      if(str[p]==L']')
        break;
      if(str[p]==L':')
      {
        isInParam = true;
        continue;
      }
      if(isInParam)
        param[pi++] = static_cast<char>(str[p]);
      else
      {
        if(ti++>1)
          return 0;
        tag=str[p];
      }
    }
    param[pi] =0;
    if(str[p]!=L']') return 0; // no closing bracket error
  }

  // ----- convert param to number
  u32 val = 0;
  if(pi>0)
  {
    if(tag==L'c')
      val = str::StrToInt(param,16);
    else if(tag==L'a')
      val = str::StrToInt(param,16);
    else 
      val = str::StrToInt(param,10);
  }

  // ----- change state
  if(tag==L'c')
    m_state.m_clr = (m_state.m_clr & 0xff000000) + val;
  if(tag==L'a')
    m_state.m_clr = SETA(m_state.m_clr,val);
  else if(tag==L'h')
    m_state.m_h = (f32)val;
  else if(tag==L'b')
    m_state.m_style = tsBold;
  else if(tag==L'i')
    m_state.m_style = tsItalic;
  else if(tag==L'u')
    m_state.m_isUnderlined = true;
  else if(tag==L's')
    m_state.m_isStroke = true;

  return p+1;
}

void TagProcessor::ClearState()
{
  m_state.m_clr = mt::COLOR_WHITE;
  m_state.m_h = 1;
  m_state.m_style = tsNormal;
  m_state.m_isUnderlined = false;
  m_state.m_isStroke = false;
}

TagProcessor::TextState& TagProcessor::GetState()
{
  return m_state;
}


// ----- LoggerFile
// ----------------------------------------------------------------------------
LoggerFile::LoggerFile( const char* fname )
{
  m_file.Open(fname,false,false);  
}

void LoggerFile::Output( const wchar_t* logStr )
{
  m_file.Write((const char*)logStr, wcslen(logStr) * sizeof(wchar_t));
}


void LoggerFile::Output( const char* logStr )
{
  m_file.Write(logStr, strlen(logStr) * sizeof(char));
}

void LoggerFile::OutputNoTags( const wchar_t* logStr, u32 color ) 
{
  NOT_USED(color);
  Output(logStr);
}

void LoggerFile::OutputNoTags( const char* logStr, u32 color ) 
{
  NOT_USED(color);
  Output(logStr);
}


//////////////////////////////////////////////////////////////////////////
 
u8 LoggerSysConsole::Color2Index(u32 clr) const 
{
  // ----- std console colors
  //0 = Black 8 = Gray
  //1 = Blue 9 = Light Blue
  //2 = Green a = Light Green
  //3 = Aqua b = Light Aqua
  //4 = Red c = Light Red
  //5 = Purple d = Light Purple
  //6 = Yellow e = Light Yellow
  //7 = White f = Bright White
  static const u32 colorTable[] = {
    0xff000000,0xff800000,0xff008000,0xff808000,
    0xff000080,0xff800080,0xff008080,0xffc0c0c0,
    0xff808080,0xffff0000,0xff00ff00,0xffffff00,
    0xff0000ff,0xffff00ff,0xff00ffff,0xffffffff
  };

  const u32 r = GETR(clr);
  const u32 g = GETG(clr);
  const u32 b = GETB(clr);
  i32 sumlen;
  i32 minSumlen = 255;
  u8 nearestIndex = 15;

  for(u8 i=0;i<16;++i)
  {
    u32 clr2 = colorTable[i];

    sumlen = abs((int)(GETR(clr2)-r)) + abs((int)(GETG(clr2)-g)) + abs((int)(GETB(clr2)-b));
    if(sumlen < minSumlen)
    {
      minSumlen = sumlen;
      nearestIndex = i;
    }
  }
  return nearestIndex;
}


LoggerSysConsole::LoggerSysConsole( const char* cname, u16 width, u16 height )
{
  ::AllocConsole();
  m_hSystemConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);

  if (m_hSystemConsole != NULL)
  {
	  ::SetConsoleTitle(cname);
	  COORD co = {(SHORT)width,(SHORT)height};
	  SetConsoleScreenBufferSize(m_hSystemConsole, co);

    if ( ::GetFileType(m_hSystemConsole) != FILE_TYPE_CHAR )
    {
      // If the debugger has jacked our console, get it back
      SECURITY_ATTRIBUTES Sec;
      Sec.nLength = sizeof(Sec);
      Sec.lpSecurityDescriptor = 0;
      Sec.bInheritHandle = TRUE;
      
      m_hSystemConsole = ::CreateFileW( 
        L"CONOUT$", 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_WRITE, 
        &Sec,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0 
      );
    }
  }
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  ::GetConsoleScreenBufferInfo(m_hSystemConsole,&csbiInfo);
  m_defaultAttribs = csbiInfo.wAttributes;
}

LoggerSysConsole::~LoggerSysConsole()
{
  CloseHandle(m_hSystemConsole);
}

void LoggerSysConsole::Output( const wchar_t* logStr )
{
  TagProcessor tagp;
  DWORD Written;
  u32 tagSkip = 0;
  u32 len = wcslen(logStr);
  for(u32 i=0;i<len;++i)
  {
    tagSkip = tagp.ParseTag(&logStr[i]);
    if(tagSkip>0)
    {
      ::SetConsoleTextAttribute( m_hSystemConsole,Color2Index(tagp.GetState().m_clr));
      i+=tagSkip;
    }
    ::WriteConsoleW(m_hSystemConsole,&logStr[i],1,&Written,0);
  }
  ::SetConsoleTextAttribute( m_hSystemConsole,m_defaultAttribs);
}

void LoggerSysConsole::OutputNoTags( const wchar_t* logStr, u32 color)
{  
   DWORD Written;
  ::SetConsoleTextAttribute( m_hSystemConsole,Color2Index(color));
  ::WriteConsoleW(m_hSystemConsole,logStr,wcslen(logStr),&Written,0);

  ::SetConsoleTextAttribute( m_hSystemConsole,m_defaultAttribs);
}


void LoggerSysConsole::OutputNoTags( const char* logStr, u32 color)
{
  OutputNoTags(str::A2W(logStr).c_str(),color);
}

void LoggerSysConsole::Output( const char* logStr )
{
  Output(str::A2W(logStr).c_str());
}

#else

LoggerSysConsole::LoggerSysConsole( const char* /*cname*/, u16 /*width*/, u16 /*height*/ )
{
}

void LoggerSysConsole::OutputNoTags( const char* logStr, u32 /*color*/)
{
  console_print_i("%s", logStr);
}

void LoggerSysConsole::Output( const char* logStr )
{
  console_print_i("%s", logStr);
}

#endif

NAMESPACE_END(dlog)