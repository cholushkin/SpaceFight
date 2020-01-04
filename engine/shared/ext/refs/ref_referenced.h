#ifndef _REFERENCED_H_
#define _REFERENCED_H_

NAMESPACE_BEGIN(ref)

class Referenced;

class RefProxy
{
  friend class Referenced;
public:
  RefProxy( const Referenced* obj );
  ~RefProxy() {}
  void                AddRef();
  void                RemRef();
  const Referenced*   Get() const;
private:
  void                OnObjectDestroy();
  const Referenced*   _object;
  int                 _refcount;
};

class Referenced
{
  template < typename TYPE > friend class WeakRef;
  template < typename TYPE > friend class SharedRef;
public:
  Referenced();

  int GetRefCount() const {
    return _sharedrefcount;
  }  
protected:
  virtual ~Referenced();

private:
  RefProxy* GetRefProxy() const;
  void      AddSharedRef();
  bool      RemSharedRef();
  virtual bool  Release();
  mutable RefProxy* _proxy;
  int       _sharedrefcount;
};



template < typename TYPE >
struct ReferencedType {
  typedef Referenced* ResultType;
};

template < typename TYPE >
struct ReferencedType< TYPE* > {
  typedef Referenced* ResultType;
};

template < typename TYPE >
struct ReferencedType< const TYPE* > {
  typedef const Referenced* ResultType;
};



inline RefProxy::RefProxy( const Referenced* obj )
  : _refcount(0)
  , _object(obj)
{  }

inline void RefProxy::AddRef() {
  ++_refcount;
}

inline void RefProxy::RemRef() {
  if (!--_refcount && !_object)
    delete this;
}

inline const Referenced* RefProxy::Get() const {
  return _object;
}

inline void RefProxy::OnObjectDestroy() {
  _object = NULL;
  if (!_refcount)
    delete this;
}


inline Referenced::Referenced()
: _proxy(NULL) 
, _sharedrefcount(0)
{}

inline Referenced::~Referenced() {
  if(_proxy)
    _proxy->OnObjectDestroy();
}

inline void Referenced::AddSharedRef()
{
  ++_sharedrefcount;
}

inline bool Referenced::RemSharedRef()
{
  --_sharedrefcount;
  return Release();
}

inline RefProxy* Referenced::GetRefProxy() const
{
  if (!_proxy)
    _proxy = new RefProxy(this);
  return _proxy;
}

inline bool Referenced::Release()
{
  if (_sharedrefcount)
    return false;
  delete this;
  return true;
}

NAMESPACE_END(ref)

#endif

