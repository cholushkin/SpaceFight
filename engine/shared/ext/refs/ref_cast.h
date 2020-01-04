#ifndef _CAST_H_
#define _CAST_H_

NAMESPACE_BEGIN(ref)

template < typename TYPETO, typename TYPEFROM >
class Conversion
{
  typedef char Yes;
  struct No
  {  
    char ar[ 2 ];
  } ;
  static Yes Test( TYPETO );
  static No  Test( ... );
  static TYPEFROM Make();
public:
  enum { Exists = sizeof( Test( Make() ) ) == sizeof( Yes ) };
  enum { Same = 0 };
};

template < typename TYPE >
class Conversion< TYPE, TYPE >
{
public:
  enum { Exists = 1 };
  enum { Same = 1 };
};

/*----------------------------------------------------------------*/

template < typename BASE, typename DERIVED >
class IsSubClassed
{
public:
  enum { Yes =  Conversion< BASE*, DERIVED* >::Exists &&
               !Conversion< BASE*, DERIVED* >::Same &&
               !Conversion< BASE*, void* >::Same };
};

template < bool >
struct CompileTimeAssert;

template < >
struct CompileTimeAssert< true >
{  };

template < typename TYPE, typename TYPE2 >
TYPE* SafeCastGlobal( TYPE2* obj )
{
  sizeof( ( CompileTimeAssert< IsSubClassed< TYPE, TYPE2 >::Yes >() ) );
  return static_cast< TYPE* >( obj );
}

template < typename TYPE, typename TYPE2 >
TYPE* DynCastGlobal( TYPE2* obj )
{
  return dynamic_cast< TYPE* >( obj );
}


/*--------------------------------------------------------------------*/

template < bool, typename Type, typename Type2 >
struct SelectType;

template < typename Type, typename Type2 >
struct SelectType< true, Type, Type2 >
{
  typedef Type ResultType;
};

template < typename Type, typename Type2 >
struct SelectType< false, Type, Type2 >
{
  typedef Type2 ResultType;
};


template<typename T>
struct IsConst {
  enum {Value = 0};
};

template<typename T>
struct IsConst<const T> {
  enum {Value = 1};
};

/*--------------------------------------------------------------------*/

NAMESPACE_END(ref)

#endif

