#ifndef _SHAREDREF_H_
#define _SHAREDREF_H_

#include "ref_cast.h"
#include "ref_referenced.h"

NAMESPACE_BEGIN(ref)

template < typename TYPE >
class WeakRef;

template < typename TYPE > 
class SharedRef 
{
public:
  /// default constructor
  SharedRef();
  /// constructor with target object
  SharedRef( TYPE* obj );
  /// template constructor with target object
  template < typename TYPE2 >
  SharedRef( TYPE2* obj );
  /// copy constructor
  SharedRef( const SharedRef< TYPE >& r );
  /// template copy constructor
  template < typename TYPE2 >
  SharedRef( const SharedRef< TYPE2 >& r );
  /// from WeakRef
  SharedRef( const WeakRef< TYPE >& r );
  /// template from WeakRef
  template < typename TYPE2 >
  SharedRef( const WeakRef< TYPE2 >& r );
  /// destructor
  ~SharedRef();

  /// assign TYPE pointer
  SharedRef< TYPE >& operator = ( TYPE *obj );
  /// template assign TYPE pointer
  template < typename TYPE2 >
  SharedRef< TYPE >& operator = ( TYPE2 *obj );
  /// assign SharedRef object
  SharedRef< TYPE >& operator = ( const SharedRef< TYPE >& r );
  /// template assign SharedRef object
  template < typename TYPE2 >
  SharedRef< TYPE >& operator = ( const SharedRef< TYPE2 >& r );
  /// assign WeakRef object
  SharedRef< TYPE >& operator = ( const WeakRef< TYPE >& r );
  /// template assign WeakRef object
  template < typename TYPE2 >
  SharedRef< TYPE >& operator = ( const WeakRef< TYPE2 >& r );

  /// equality operator
  bool operator == ( TYPE* obj ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( TYPE2* obj ) const;
  /// equality operator
  bool operator == ( const SharedRef< TYPE >& r ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( const SharedRef< TYPE2 >& r ) const;
  /// equality operator
  bool operator == ( const WeakRef< TYPE >& r ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( const WeakRef< TYPE2 >& r ) const;  

  /// inequality operator
  bool operator != ( TYPE* obj ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( TYPE2* obj ) const; 
  /// inequality operator
  bool operator != ( const SharedRef< TYPE >& r ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( const SharedRef< TYPE2 >& r ) const;
  /// inequality operator
  bool operator != ( const WeakRef< TYPE >& r ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( const WeakRef< TYPE2 >& r ) const;  

  /// ! operator
  bool operator ! ()  const;
  /// override -> operator
  TYPE* operator -> () const;
  /// dereference operator
  TYPE& operator * () const;

  /// set target object
  void Set( TYPE *obj );
  /// get target object (unsafe)
  TYPE* Get() const;
  /// release
  bool Release();
  /// check WeakRef
  bool IsValid() const;

  template < typename TYPE2 >
  SharedRef< TYPE2 > DynCast() const;
 
private:
  
  TYPE* _object;
};

template < typename TYPE > 
inline SharedRef< TYPE >::SharedRef()
  : _object( NULL )
{  }

template < typename TYPE >
inline SharedRef< TYPE >::SharedRef( TYPE* obj )
  : _object( NULL )
{
  Set( obj );
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >::SharedRef( TYPE2* obj )
  : _object( NULL )
{
  Set( SafeCastGlobal< TYPE >( obj ) );
}

template < typename TYPE >
inline SharedRef< TYPE >::SharedRef( const SharedRef< TYPE >& r )
  : _object( NULL )
{
  Set( r.Get() );
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >::SharedRef( const SharedRef< TYPE2 >& r )
  : _object( NULL )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
}

template < typename TYPE >
inline SharedRef< TYPE >::SharedRef( const WeakRef< TYPE >& r )
  : _object( NULL )
{
  Set( r.Get() );
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >::SharedRef( const WeakRef< TYPE2 >& r )
  : _object( NULL )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
}

template < typename TYPE >
inline SharedRef< TYPE >::~SharedRef()
{
  Release();
}

template < typename TYPE >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( TYPE* obj )
{
  Set( obj );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( TYPE2* obj )
{
  Set( SafeCastGlobal< TYPE >( obj ) );
  return *this;
}

template < typename TYPE >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( const SharedRef< TYPE >& r )
{
  Set( r.Get() );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( const SharedRef< TYPE2 >& r )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
  return *this;
}

template < typename TYPE >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( const WeakRef< TYPE >& r )
{
  Set( r.Get() );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE >& SharedRef< TYPE >::operator = ( const WeakRef< TYPE2 >& r )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
  return *this;
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator == ( TYPE* obj ) const
{
  return Get() == obj;
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator == ( TYPE2* obj ) const 
{
  return Get() == obj;
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator == ( const SharedRef< TYPE >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator == ( const SharedRef< TYPE2 >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator == ( const WeakRef< TYPE >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator == ( const WeakRef< TYPE2 >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator != ( TYPE* obj ) const
{
  return !( *this == obj );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator != ( TYPE2* obj ) const
{
  return !( *this == obj );
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator != ( const SharedRef< TYPE >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator != ( const SharedRef< TYPE2 >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator != ( const WeakRef< TYPE >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool SharedRef< TYPE >::operator != ( const WeakRef< TYPE2 >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
inline bool SharedRef< TYPE >::operator ! () const
{
  return !Get();
}

template < typename TYPE >
inline TYPE* SharedRef< TYPE >::operator -> () const
{
  EASSERT(_object); // "Null pointer access through SharedRef!"
  return Get();
}

template < typename TYPE >
inline TYPE& SharedRef< TYPE >::operator * () const
{
  EASSERT(_object);//, L"Null pointer access through SharedRef!");
  return *Get();
}

template < typename TYPE >
inline void SharedRef< TYPE >::Set( TYPE* obj )
{
  if(_object == obj)
    return;
  Release();
  _object = obj;
  if (_object) 
    const_cast< Referenced* >( static_cast< typename ReferencedType< TYPE* >::ResultType >( _object ) )->AddSharedRef();
}

template < typename TYPE >
inline TYPE* SharedRef< TYPE >::Get() const
{    
  return _object;
}

template < typename TYPE >
inline bool SharedRef< TYPE >::Release()
{
  bool result = false;
  if (_object) 
  {
    result = const_cast< Referenced* >( static_cast< typename ReferencedType< TYPE* >::ResultType >( _object ) )->RemSharedRef();
    _object = NULL;
  }
  return result;
}

template < typename TYPE >
inline bool SharedRef< TYPE >::IsValid() const
{
  return !!Get(); 
}

template < typename TYPE >
template < typename TYPE2 >
inline SharedRef< TYPE2 > SharedRef< TYPE >::DynCast() const
{
  return SharedRef< TYPE2 >( dynamic_cast< TYPE* >( _object ) );
}

NAMESPACE_END(ref)

#endif

