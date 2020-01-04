#ifndef tstring_h 
#define tstring_h

#include "ext/strings/str_stringbuilder.h"

NAMESPACE_BEGIN(str)

inline u32 string_len(const wchar_t* const str )
{
  u32 l = 0;
  while(/*NULL != */str[l])
    ++l;
  return l;
}

template<class T>
class String
{
  T*     m_str;
  size_t m_size;
  size_t m_buffsize;

  void freemem()
  {
    delete [] m_str;
    m_str      = NULL;
    m_size     = 0;
    m_buffsize = 0;
  }

  void alloc(size_t len)
  {
    freemem();
    m_buffsize = len;
    m_str = new T[m_buffsize * sizeof(T)];
    m_str[0] = 0;
  }

  void ensure(size_t len)
  {
    if(len + 1 <= m_buffsize)
      return;
    alloc(len + 1);
  }

  void grow(size_t len)
  {
    if(len <= m_buffsize)
      return;
    T* buf = new T[len * sizeof(T)];
    memcpy(buf, m_str, (size() + 1) * sizeof(T));
    delete [] m_str;
    m_str = buf;
    m_buffsize = len;
  }

public:
  typedef T&       reference;
  typedef const T& const_reference;
  typedef size_t   size_type;
  typedef T        value_type;
  static const size_t npos = size_t(-1);

  bool empty() const
  {
    return 0 == m_buffsize;
  }

  void clear()
  {
    alloc(1);
  }

  void assign(const T* str)
  {
    if(str)
      assign(str, strlength(str));
    else
      alloc(1);
  }

  void assign(const T* str, size_t len)
  {
    ensure(len);
    memcpy(m_str, str, len * sizeof(T));
    m_str[len] = (T)NULL;
    m_size     = len;
  }

  void resize(size_t len)
  {
    grow(len + 1);
    m_str[len] = (T)NULL;
    m_size     = len;
  }

  void reserve(size_t len)
  {
    ensure(len + 1);
  }

  size_t find(T c) const
  {
    const size_t l = size();
    for(size_t i = 0; l != i; ++i)
      if(m_str[i] == c)
        return i;
    return npos;
  }

  size_t size() const
  {
    return m_size;
  }

  size_t find(const T* str) const
  {
    return find(str, 0);
  }

  size_t find(const T* str, size_t start) const
  {
    const T* from = m_str + start;
    const size_t l_self  = strlength(from);
    const size_t l_other = strlength(str);

    if(l_self < l_other)
      return npos;

    if((l_self == l_other) && (*this == str))
      return 0;

    for(size_t i = 0; l_self - l_other + 1 != i; ++i)
      if(0 == memcmp(from + i, str, sizeof(T) * l_other))
        return start + i;
    return npos;
  }

  // iterators
  T* end()
  {
    return m_str + size();
  }

  const T* end() const
  {
    return m_str + size();
  }

  T* begin()
  {
    return m_str;
  }

  const T* begin() const
  {
    return m_str;
  }

  String substr(size_t from, size_t l = npos) const
  {
    if(npos == l)
      l = size() - from;
    String res;
    res.assign(&m_str[from], l);
    return res;
  }

  String(const T* str)
    : m_str(NULL), m_size(0), m_buffsize(0)
  {
    assign(str);
  }

  String(const T* str, const T* tail)
    : m_str(NULL), m_size(0), m_buffsize(0)
  {
    assign(str, tail - str);
  }

  String()
    : m_str(NULL), m_size(0), m_buffsize(0)
  {
    alloc(1);
  }

  String(const String& other)
    : m_str(NULL), m_size(0), m_buffsize(0)
  {
    assign(other.m_str);
  }

  T& operator[] (size_t pos)
  {
    return m_str[pos];
  };

  const T& operator[] (size_t pos) const
  {
    return m_str[pos];
  };

  const T* c_str() const
  {
    return m_str;
  }

  String& operator=(const String& s)
  {
    assign(s.m_str);
    return *this;
  }

  String& operator=(const T* str)
  {
    assign(str);
    return *this;
  }

  ~String()
  {
    freemem();
  }

  void push_back(T t)
  {
    size_t self = size();
    if(m_buffsize <= self + 1)
      grow(self + 1 + 1);
    m_str[self] = t;
    m_str[self + 1] = (T)NULL;
    ++m_size;
  }

  String& operator+=(const String& str)
  {
    return (*this) += str.m_str;
  }

  String& operator+=(const T* str)
  {
    size_t append = strlength(str);
    size_t self   = size();
    if(m_buffsize <= self + append)
      grow(self + append + 1);
    memcpy(m_str + self, str, append + 1);
    m_size  = self + append;
    return *this;
  }

  String operator+(const String& str) const
  {
    return String(*this) += str;
  }

  String operator+(const T* str) const
  {
    return String(*this) += str;
  }

  bool operator==(const String& str) const
  {
    return *this == (str.c_str());
  }

  bool operator<(const String& r) const
  {
    // $$$ non-standard compare!
    size_t l1 = size();
    size_t l2 = r.size();
    if(l1 == l2)
      return memcmp(c_str(), r.c_str(), l2 * sizeof(T)) < 0;
    return l1 < l2;
  }

  bool begins_with(const T* str) const
  {
    size_t l1 = size();
    size_t l2 = strlength(str);
    if(l1 < l2)
      return false;
    return 0 == memcmp(m_str, str, l2 * sizeof(T));
  }

  static size_t strlength(const T* str);
};

// kinda traits
template<> inline size_t String<char>::strlength(const char* str)
{
  return size_t(strlen(str));
}

template<> inline size_t String<wchar_t>::strlength(const wchar_t* str)
{
  return size_t(wcslen(str));
}

template<class T> bool operator==(const String<T>& str, const T* szstr)
{
  size_t l1 = str.size();
  size_t l2 = String<T>::strlength(szstr);
  if(l1 != l2)
    return false;
  return 0 == memcmp(str.c_str(), szstr, l1 * sizeof(T));
}

template<class T> bool operator!=(const String<T>& str, const T* szstr)
{
  return !(str == szstr);
}

template<class T> bool operator==(const T* szstr, const String<T>& str)
{
  return str == szstr;
}

template<class T> bool operator!=(const T* szstr, const String<T>& str)
{
  return str != szstr;
}

typedef String<char>     StringA;
typedef String<wchar_t>  StringW;

typedef StringBuilderS< char  , StringA> StringBuilderA;
typedef StringBuilderS<wchar_t, StringW> StringBuilderW;


NAMESPACE_END(str)

#endif // tstring_h 
