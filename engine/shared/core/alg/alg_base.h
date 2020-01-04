#ifndef alg_base_h__
#define alg_base_h__

#include <cstddef>
#include "core/common/com_misc.h"

NAMESPACE_BEGIN(alg)

//////////////////////////////////////////////////////////////////////////
// INode

template <class T>
class INode
{
public:
  static size_t count() { return m_count;     }
  static T*     first() { return (T*)m_first; } // can get rid of cast, but need other constructor
  T*       next() { return (T*)m_next;  }
  T const* next() const { return (T const*)m_next; }

protected:
  INode()
    : m_next(m_first)
  {
    ++m_count;
    m_first = this;
  }

  virtual ~INode()
  {
    if(this == m_first)
      m_first = m_next;
    else
    {
      INode* n = m_first;
      while(n && n->m_next != this)
        n = n->m_next;
      if(n)
        n->m_next = m_next;
    }
    --m_count;
  }

private:
  INode*        m_next;
  static INode* m_first;
  static size_t m_count;

  INode& operator=(const INode& other);
  INode(const INode&);
};

#define IMPLEMENT_NODE(TYPE) \
  template<> size_t            alg::INode<TYPE>::m_count = 0; \
  template<> alg::INode<TYPE>* alg::INode<TYPE>::m_first = NULL;

// be sure not to call delete this while in iteration!
template<class T>
static void for_each(void(T::*ptr)()) {
  T* ti = alg::INode<T>::first();
  while(ti)
  {
    (ti->*ptr)();
    ti = ti->next();
  }
}

// be sure not to call delete this while in iteration!
template<class T, class A1>
static void for_each(void(T::*ptr)(const A1& a1), const A1& a1) {
  T* ti = alg::INode<T>::first();
  while(ti)
  {
    (ti->*ptr)(a1);
    ti = ti->next();
  }
}

//////////////////////////////////////////////////////////////////////////
// ArrayPtr

template<class T>
class ArrayPtr
{
  T* m_arr;
  // $$$ size_t size;
  ArrayPtr(const ArrayPtr&);
  ArrayPtr& operator=(const ArrayPtr&);
public:
  ArrayPtr()
    : m_arr(NULL){}
  ArrayPtr(T* arr)
    : m_arr(arr){}

  operator T* (){
    return get();
  }

  operator const T* () const {
    return get();
  }

  T* get() {
    return m_arr;
  }

  const T* get() const {
    return m_arr;
  }

  void reset(T* arr) {
    delete [] m_arr;
    m_arr = arr;
  }

  T* withdraw() {
    T* res = m_arr;
    m_arr = NULL;
    return res;
  }

  ~ArrayPtr(){
    delete [] m_arr;
    m_arr = NULL;
  }
};

NAMESPACE_END(alg)
#endif // alg_base_h__