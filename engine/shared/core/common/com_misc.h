#ifndef _com_misc_h
#define _com_misc_h

#include "core/common/com_types.h"

#define SAFE_DELETE( P )       { delete (P); (P) = NULL; }
#define SAFE_DELETE_ARRAY( P ) { delete[] (P); (P) = NULL; }
#define DELETE_VECTOR_ELEMENTS(v) for(u32 i__=0;i__<v.size();++i__) delete v[i__];
#define VECTOR_ITERATE(ITER_NAME,VT,V) for( std::vector<VT>::iterator ITER_NAME = V.begin(); ITER_NAME != V.end(); ++ITER_NAME)
#define VECTOR_REVERSE_ITERATE(ITER_NAME,VT,V) for( std::vector<VT>::reverse_iterator ITER_NAME = V.rbegin(); ITER_NAME != V.rend(); ++ITER_NAME)
#define VECTOR_ITERATE_CONST(ITER_NAME,VT,V) for( std::vector<VT>::const_iterator ITER_NAME = V.begin(); ITER_NAME != V.end(); ++ITER_NAME)
#define LIST_ITERATE(ITER_NAME,LT,L) for( std::list<LT>::iterator ITER_NAME = L.begin(); ITER_NAME != L.end(); ++ITER_NAME)
#define LIST_ITERATE_CONST(ITER_NAME,LT,L) for( std::list<LT>::const_iterator ITER_NAME = L.begin(); ITER_NAME != L.end(); ++ITER_NAME)
#define DELETE_LIST_ELEMENTS(v) for(u32 i__=0;i__<v.size();++i__) delete v[i__];

#define Stringize( L )		 #L
#define MakeString( M, L ) M(L)
#define $Line					     MakeString( Stringize, __LINE__ )
#define REMINDER( L ) message(__FILE__ "(" $Line ") : REMINDER > "  L )

#define ARRAY_SIZE( array ) ( sizeof(array) / sizeof(array[0]) ) 
#define STATIC_ASSERT(PRED) {int test[PRED]; test;}

#define DENY_COPY( className ) private:\
  className( const className& );\
  className& operator= ( const className& );

#define NOT_USED(x)x
#define NAMESPACE_BEGIN(x) namespace x {
#define NAMESPACE_END(x) }

  


//////////////////////////////////////////////////////////////////////////
// Enum helpers

// this macro allows you to use a typesafe bitflag parameter as a strict enum
// rather than simple an int or dword. the operators need to be defined so you
// can combine and test the component flags.
//
// note that we have a NOT() rather than ~() (bitwise-not) because the compiler
// can't figure it out, colliding with dtors. $$ was this fixed or is there a
// new workaround available in vs2005?
#define MAKE_ENUM_BIT_OPERATORS( inEnumType )                \
  inline inEnumType operator | ( inEnumType a, inEnumType b )     \
{  return (inEnumType)((int)a | (int)b); }                        \
  inline inEnumType operator & ( inEnumType a, inEnumType b )     \
{  return (inEnumType)((int)a & (int)b); }                        \
  inline inEnumType operator |= ( inEnumType& a, inEnumType b )   \
{  return a = a | b; }                                            \
  inline inEnumType operator &= ( inEnumType& a, inEnumType b )   \
{  return a = a & b; }                                            \
  inline inEnumType NOT( inEnumType a )                           \
{  return (inEnumType)~(int)a; }

// this macro allows you to use math operations on enums and still be
// relatively typesafe. add more as you find the need.
#define MAKE_ENUM_MATH_OPERATORS( inEnumType )                               \
  inline inEnumType operator ++ ( inEnumType& a )         /* prefix increment */  \
{  return a = (inEnumType)(a + 1);  }                                             \
  inline inEnumType operator ++ ( inEnumType& a, int )    /* postfix increment */ \
{  inEnumType old = a;  ++a;  return old;  }                                      \
  inline inEnumType operator -- ( inEnumType& a )         /* prefix decrement */  \
{  return a = (inEnumType)(a - 1);  }                                             \
  inline inEnumType operator -- ( inEnumType& a, int )    /* postfix decrement */ \
{  inEnumType old = a;  --a;  return old;  }                                      \
  inline inEnumType operator + ( inEnumType a, inEnumType b )                     \
{  return (inEnumType)((int)a + (int)b); }                                        \
  inline inEnumType operator - ( inEnumType a, inEnumType b )                     \
{  return (inEnumType)((int)a - (int)b); }                                        \
  inline inEnumType operator += ( inEnumType& a, inEnumType b )                   \
{  return a = (inEnumType)(a + b); }                                              \
  inline inEnumType operator -= ( inEnumType& a, inEnumType b )                   \
{  return a = (inEnumType)(a - b); }


//////////////////////////////////////////////////////////////////////////
// Text Box
// todo: move to ext/gui
// C-style
typedef void(pOnTextBoxResult)(bool);
void TextBox_SetHook(pOnTextBoxResult*);

// C++-style
class TextBoxListener
{
public:
  TextBoxListener();

protected:
  ~TextBoxListener();
  void TextBox_CallInPlace(const wchar_t * const szTitle,       wchar_t* buffer, unsigned int size); // buffer is provided
  void TextBox_CallAlloc  (const wchar_t * const szTitle, const wchar_t* buffer, unsigned int size); // buffer is created
  virtual void TextBox_OnResult(bool, const wchar_t*){}

private:
  static TextBoxListener* g_listener;
  static void sOnResult(bool);

  // note: if 0 == m_own_size then we do not own the m_buffer!
  wchar_t*     m_buffer;
  unsigned int m_own_size;

  void DestroyBuffer();
  void Hook();
  void UnHook();
  void OnResult(bool);
};

#endif // _com_misc_h
