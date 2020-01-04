#ifndef str_stringbuilder_h__
#define str_stringbuilder_h__

#include "core/common/com_misc.h"
#include "core/math/mt_base.h"
#include <string.h>

NAMESPACE_BEGIN(str)

//  Converts decimal number to it's string representation
inline u32 dectostring( wchar_t* out, u32 value )
{
  const wchar_t decchars[] = L"0123456789";
  wchar_t buffer[11];

  if( 0 == value ) {
    out[0] = L'0';
    out[1] = 0;
    return 1;
  }

  u32 val = value;
  u32 i = 0;

  while( val > 0 ) {
    buffer[i] = decchars[val % 10];
    val /= 10;
    i++;
  }
  for( u32 j = 0; j < i; j++ )
    out[ i - j - 1 ] = buffer[ j ];
  out[i] = 0;
  return i;
}

u32 StrToInt(const char* str, u8 base);


//////////////////////////////////////////////////////////////////////////
// StringBuilder

template<typename T>
struct StringBuilderCharSet
{
  static T minus  ()  { return data[s_minus_offset  ]; }
  static T dot    ()  { return data[s_dot_offset    ]; }
  static T percent()  { return data[s_percent_offset]; }

  static T at(size_t t) { EASSERT(t < 10); return data[t];}
private:
  static const T data[];
  static const size_t s_minus_offset   = 11;
  static const size_t s_dot_offset     = 12;
  static const size_t s_percent_offset = 13;
};

template <class FL>
struct StringBuilderFormatFloat
{
  StringBuilderFormatFloat(FL f, size_t prec = 3, bool cut = false)
    : f(f)
    , prec(prec)
    , cut(cut)
  {}
  FL     f;
  size_t prec;
  bool   cut;
};

template<typename  T, size_t buff_size>
class StringBuilderFixedBuffer
{
public:
  StringBuilderFixedBuffer()
    : m_pos(0)
  {
      end();
  }

  operator const T* () const
  {
    return get();
  }

  const T* get() const
  {
    return &m_buff[0];
  }

  void clear()
  {
    m_pos = 0;
    end();
  }

  void put(T c)
  {
    if(m_pos != buff_size - 1)
      m_buff[m_pos++] = c;
  }

  void end()
  {
    m_buff[m_pos] = 0;
  }

private:
  T      m_buff[buff_size];
  size_t m_pos;
};

template<typename STR, typename T>
class StringBuilderStr
{
public:

  operator const STR& () const
  {
    return m_buff;
  }

  operator const T* () const
  {
    return get();
  }

  const T* get() const
  {
    return m_buff.c_str();
  }

  void clear()
  {
    m_buff.clear();
  }

  void put(T c)
  {
    m_buff.push_back(c);
  }

  void end()
  {}

private:
  STR    m_buff;
};

template<class Buffer, class T>
class StringBuilder
{
public:

  operator const T* () const { return Get(); }
  const T* Get() const { return m_buff.get(); }
  StringBuilder& clear(){ m_buff.clear(); return *this;}

  StringBuilder& operator()(int si)
  {
    if(si < 0) 
    {
      put(StringBuilderCharSet<T>::minus());
      si = -si;
    }
    print_dec(si, 0);
    return *this;
  }

  StringBuilder& operator()(unsigned int ui)
  {
    print_dec(ui, 0);
    return *this;
  }

  StringBuilder& operator()(unsigned int ui, unsigned int fill_zeroes)
  {
    print_dec(ui, fill_zeroes);
    return *this;
  }

  Buffer& GetBuffer()
  {
    return m_buff; 
  }

  const Buffer& GetBuffer() const
  {
    return m_buff; 
  }

  // Floats
  StringBuilder& operator()(float val)
  {
    return (*this)((double)val);
  }

  StringBuilder& operator()(float val, size_t precision, bool cut = false)
  {
    return (*this)((double)val, precision, cut);
  }

  StringBuilder& operator()(double val)
  {
    return (*this)(val, 3, true);
  }

  StringBuilder& operator()(double val, size_t precision, bool cut = false)
  {
    if(val < 0)
    {
      put(StringBuilderCharSet<T>::minus());
      val = -val;
    }

    double i(0);
    double f = modf(val, &i);

    print_dec((unsigned int)i, 0);
    put(StringBuilderCharSet<T>::dot());

    while (0 != precision--)
      f *= 10;

    print_dec((unsigned int)mt::round(f), 0, cut);

    return *this;
  }

  template <class FL>
  StringBuilder& operator()(const StringBuilderFormatFloat<FL>& val)
  {
    return (*this)(val.f, val.prec, val.cut);
  }

  StringBuilder& operator()(T c)
  {
    put_end(c);
    return *this;
  }

  StringBuilder& operator()(const T* s)
  {
    while(*s)
      put(*s++);
    end();
    return *this;
  }

  // Formatters

  template<class T0>
  StringBuilder& operator()(const T* s, T0 t0)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }

  template<class T0, class T1>
  StringBuilder& operator()(const T* s, T0 t0, T1 t1)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else if(code == StringBuilderCharSet<T>::at(1))
          (*this)(t1);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }

  template<class T0, class T1, class T2>
  StringBuilder& operator()(const T* s, T0 t0, T1 t1, T2 t2)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else if(code == StringBuilderCharSet<T>::at(1))
          (*this)(t1);
        else if(code == StringBuilderCharSet<T>::at(2))
          (*this)(t2);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }

  template<class T0, class T1, class T2, class T3>
  StringBuilder& operator()(const T* s, T0 t0, T1 t1, T2 t2, T3 t3)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else if(code == StringBuilderCharSet<T>::at(1))
          (*this)(t1);
        else if(code == StringBuilderCharSet<T>::at(2))
          (*this)(t2);
        else if(code == StringBuilderCharSet<T>::at(3))
          (*this)(t3);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }

  template<class T0, class T1, class T2, class T3, class T4>
  StringBuilder& operator()(const T* s, T0 t0, T1 t1, T2 t2, T3 t3, T4 t4)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else if(code == StringBuilderCharSet<T>::at(1))
          (*this)(t1);
        else if(code == StringBuilderCharSet<T>::at(2))
          (*this)(t2);
        else if(code == StringBuilderCharSet<T>::at(3))
          (*this)(t3);
        else if(code == StringBuilderCharSet<T>::at(4))
          (*this)(t4);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }

  template<class T0, class T1, class T2, class T3, class T4, class T5>
  StringBuilder& operator()(const T* s, T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
  {
    while(*s)
    {
      if(StringBuilderCharSet<T>::percent() == *s && *(s + 1))
      {
        const T code = *(++s);
        if(code == StringBuilderCharSet<T>::at(0))
          (*this)(t0);
        else if(code == StringBuilderCharSet<T>::at(1))
          (*this)(t1);
        else if(code == StringBuilderCharSet<T>::at(2))
          (*this)(t2);
        else if(code == StringBuilderCharSet<T>::at(3))
          (*this)(t3);
        else if(code == StringBuilderCharSet<T>::at(4))
          (*this)(t4);
        else if(code == StringBuilderCharSet<T>::at(5))
          (*this)(t5);
        else
          (*this)(code);
      }
      else
        put(*s);
      ++s;
    }
    end();
    return *this;
  }
private:
  Buffer m_buff;

  void print_dec(int value, int fill_zeroes, bool trim_end_zeroes = false)
  {
    if( 0 == value )
    {
      while(--fill_zeroes > 0)
        put(StringBuilderCharSet<T>::at(0));
      return put_end(StringBuilderCharSet<T>::at(0));
    }

    T buffer[11];
    size_t val = value;
    size_t i = 0;
    do
    {
      buffer[i++] = StringBuilderCharSet<T>::at(val % 10);
    }
    while( (val /= 10) > 0 );

    if(fill_zeroes > 0)
      while(fill_zeroes-- > (int)i)
        put(StringBuilderCharSet<T>::at(0));
    
    size_t trim = i;
    if(trim_end_zeroes)
      for(size_t z = 0; z != i; ++z)
        if(buffer[z] == StringBuilderCharSet<T>::at(0))
          --trim;
        else
          break;

    for(size_t j = 0; j < trim; ++j )
        put(buffer[i - j - 1]);

    end();
  }

  void put(T c)
  {
    m_buff.put(c);
  }

  void end()
  {
    m_buff.end();
  }

  void put_end(T c)
  {
    m_buff.put(c);
    m_buff.end();
  }
};

template<typename  T, size_t buff_size>
class StringBuilderF : public StringBuilder< StringBuilderFixedBuffer<T, buff_size>, T >
{};

template<typename  T, typename STR>
class StringBuilderS : public StringBuilder< StringBuilderStr<STR, T>, T >
{};

NAMESPACE_END(str)

#endif // str_stringbuilder_h__