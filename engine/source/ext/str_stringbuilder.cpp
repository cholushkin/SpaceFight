#include "ext/strings/str_stringbuilder.h"

NAMESPACE_BEGIN(str)

template<> const wchar_t StringBuilderCharSet<wchar_t>::data[] = L"0123456789+-.%";
template<> const char    StringBuilderCharSet< char  >::data[] =  "0123456789+-.%";

u32 StrToInt(const char* str, u8 base)
{
  return strtol( str, NULL, base );
}

NAMESPACE_END(str)