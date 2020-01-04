#ifndef _WEAKREF_H_
#define _WEAKREF_H_

#include "ext/refs/ref_cast.h"
#include "ext/refs/ref_referenced.h"

NAMESPACE_BEGIN(ref)

template < typename TYPE >
class SharedRef;

template < typename TYPE > 
class WeakRef 
{
public:
  /// default constructor
  WeakRef();
  /// constructor with target object
  WeakRef( TYPE* obj );
  // template constructor with target object
  template < typename TYPE2 >
  explicit WeakRef( TYPE2* obj ); 
  /// copy constructor
  WeakRef( const WeakRef< TYPE >& r );
  /// template copy constructor
  template < typename TYPE2 >
  WeakRef( const WeakRef< TYPE2 >& r );
  /// from SharedRef
  WeakRef( const SharedRef< TYPE >& r );
  /// from SharedRef template
  template < typename TYPE2 >
  WeakRef( const SharedRef< TYPE2 >& r );
  /// destructor
  ~WeakRef();

  /// assign TYPE pointer
  WeakRef< TYPE >& operator = ( TYPE *obj );
  /// template assign TYPE pointer
  template < typename TYPE2 >
  WeakRef< TYPE >& operator = ( TYPE2 *obj );
  /// assign WeakRef object
  WeakRef< TYPE >& operator = ( const WeakRef< TYPE >& r );
  /// template assign WeakRef object
  template < typename TYPE2 >
  WeakRef< TYPE >& operator = ( const WeakRef< TYPE2 >& r );
  /// assign SharedRef object
  WeakRef< TYPE >& operator = ( const SharedRef< TYPE >& r );
  /// template assign SharedRef object
  template < typename TYPE2 >
  WeakRef< TYPE >& operator = ( const SharedRef< TYPE2 >& r );

  /// equality operator
  bool operator == ( TYPE* obj ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( TYPE2* obj ) const;
  /// equality operator
  bool operator == ( const WeakRef< TYPE >& r ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( const WeakRef< TYPE2 >& r ) const;  
  /// equality operator
  bool operator == ( const SharedRef< TYPE >& r ) const;
  /// template equality operator
  template < typename TYPE2 >
  bool operator == ( const SharedRef< TYPE2 >& r ) const;

  /// inequality operator
  bool operator != ( TYPE* obj ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( TYPE2* obj ) const; 
  /// inequality operator
  bool operator != ( const WeakRef< TYPE >& r ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( const WeakRef< TYPE2 >& r ) const;  
  /// inequality operator
  bool operator != ( const SharedRef< TYPE >& r ) const;
  /// template inequality operator
  template < typename TYPE2 >
  bool operator != ( const SharedRef< TYPE2 >& r ) const;
  
  /// ! operator
  bool operator ! ()  const;
  /// override -> operator
  TYPE* operator -> () const;
  /// dereference operator
  TYPE& operator * () const;

  /// set target object
  void Set( TYPE* obj );
  /// get target object (unsafe)
  TYPE* Get() const;
  /// release
  bool Release();
  /// invalidate WeakRef
  void Invalidate();
  /// check WeakRef
  bool IsValid() const;

  template < typename TYPE2 >
  WeakRef< TYPE2 > DynCast() const;

private:

  RefProxy* _proxy;
};

template < typename TYPE > 
inline WeakRef< TYPE >::WeakRef()
  : _proxy( NULL )
{  }

template < typename TYPE >
inline WeakRef< TYPE >::WeakRef( TYPE* obj )
  : _proxy( NULL )
{
  Set( obj );
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >::WeakRef( TYPE2* obj )
  : _proxy( NULL )
{
  Set( SafeCastGlobal< TYPE >( obj ) );
}

template < typename TYPE >
inline WeakRef< TYPE >::WeakRef( const WeakRef< TYPE >& r )
  : _proxy( NULL )
{
  Set( r.Get() );
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >::WeakRef( const WeakRef< TYPE2 >& r )
  : _proxy( NULL )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
}

template < typename TYPE >
inline WeakRef< TYPE >::WeakRef( const SharedRef< TYPE >& r )
  : _proxy( NULL )
{
  Set( r.Get() );
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >::WeakRef( const SharedRef< TYPE2 >& r )
  : _proxy( NULL )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
}

template < typename TYPE >
inline WeakRef< TYPE >::~WeakRef()
{
  Invalidate();
}

template < typename TYPE >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( TYPE* obj )
{
  Set( obj );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( TYPE2* obj )
{
  Set( SafeCastGlobal< TYPE >( obj ) );
  return *this;
}

template < typename TYPE >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( const WeakRef< TYPE >& r )
{
  Set( r.Get() );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( const WeakRef< TYPE2 >& r )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
  return *this;
}

template < typename TYPE >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( const SharedRef< TYPE >& r )
{
  Set( r.Get() );
  return *this;
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE >& WeakRef< TYPE >::operator = ( const SharedRef< TYPE2 >& r )
{
  Set( SafeCastGlobal< TYPE >( r.Get() ) );
  return *this;
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator == ( TYPE* obj ) const
{
  return Get() == obj;
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator == ( TYPE2* obj ) const
{
  return Get() == obj;
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator == ( const WeakRef< TYPE >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator == ( const WeakRef< TYPE2 >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator == ( const SharedRef< TYPE >& r ) const
{
  return Get() == r.Get();
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator == ( const SharedRef< TYPE2 >& r ) const
{
	return Get() == r.Get();
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator != ( TYPE* obj ) const
{
  return !( *this == obj );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator != ( TYPE2* obj ) const
{
  return !( *this == obj );
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator != ( const WeakRef< TYPE >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator != ( const WeakRef< TYPE2 >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator != ( const SharedRef< TYPE >& r ) const
{
  return !( *this == r );
}

template < typename TYPE >
template < typename TYPE2 >
inline bool WeakRef< TYPE >::operator != ( const SharedRef< TYPE2 >& r ) const
{
	return !( *this == r );
}

template < typename TYPE >
inline bool WeakRef< TYPE >::operator ! () const
{
  return !Get();
}

template < typename TYPE >
inline TYPE* WeakRef< TYPE >::operator -> () const
{
  EASSERT(_proxy, L"Null pointer access through WeakRef!");
  return Get();
}

template < typename TYPE >
inline TYPE& WeakRef< TYPE >::operator * () const
{
  EASSERT(_proxy, L"Null pointer access through WeakRef!");
  return *Get();
}

template < typename TYPE >
inline void WeakRef< TYPE >::Set( TYPE* obj )
{
  Invalidate();
  if (!obj)
    return;
  _proxy = (static_cast< typename ReferencedType< TYPE* >::ResultType >( obj ))->GetRefProxy();
  _proxy->AddRef();
}

template<class TYPE>
inline TYPE* WeakRef<TYPE>::Get() const
{    
  if (_proxy)
  {
    return static_cast< TYPE* >( const_cast< Referenced* >( _proxy->Get() ) );
  }
  else
    return NULL;
}

template < typename TYPE >
inline bool WeakRef< TYPE >::Release()
{
  if (IsValid()) 
    return Get()->Release();
  return false;
}

template < typename TYPE >
inline void WeakRef< TYPE >::Invalidate()
{
  if (_proxy) 
  {
    _proxy->RemRef();
    _proxy = NULL;
  }
}

template < typename TYPE >
inline bool WeakRef< TYPE >::IsValid() const
{
  return !!Get(); 
}

template < typename TYPE >
template < typename TYPE2 >
inline WeakRef< TYPE2 > WeakRef< TYPE >::DynCast() const
{
  return WeakRef< TYPE2 >( DynCastGlobal< TYPE2 >( Get() ) );
}

NAMESPACE_END(ref)

#endif

