#ifndef str_conv_h 
#define str_conv_h

#include <string>
#include "ext/strings/str_base.h"
#include <locale>
#include <codecvt>

#pragma warning(push)
#pragma warning(disable:4244) // 'argument' : conversion from 'utf8::uint32_t' to 'const wchar_t'
#include "ext/utf8/utf8.h"
#pragma warning(pop)

NAMESPACE_BEGIN(str)

typedef std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>> ascii_conv;

inline std::wstring A2W( const std::string &s)
{
  return ascii_conv().from_bytes(s);
}

inline std::string W2A( const std::wstring &s)
{
  return ascii_conv().to_bytes(s);
}

template<size_t> StringW fromUTF8(const StringA& utf8);

template<>
inline StringW fromUTF8<2>(const StringA& utf8){
  StringW res; res.reserve(utf8.size());
  utf8::unchecked::utf8to16(utf8.begin(), utf8.end(), std::back_insert_iterator< StringW >(res));
  return res;
}

template<>
inline StringW fromUTF8<4>(const StringA& utf8){
  StringW res; res.reserve(utf8.size());
  utf8::unchecked::utf8to32(utf8.begin(), utf8.end(), std::back_insert_iterator< StringW >(res));
  return res;
}

inline StringW fromUTF8(const char* utf8) {
  return fromUTF8<sizeof(wchar_t)>(StringA(utf8));
}

NAMESPACE_END(str)

#endif // str_conv_h 
