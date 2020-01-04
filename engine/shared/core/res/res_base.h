#ifndef res_base_h__
#define res_base_h__

#include "core/common/com_misc.h"

NAMESPACE_BEGIN(res)

typedef void(*FreeResourceFunct)(const void*);
typedef u32 ResID;

struct ResourceTableEntry {
  ResID       m_id;
  const char* m_name;
};

class ResourcesPool {
  DENY_COPY(ResourcesPool)
  template<class T>
  static void GenericFreeResource(const void* p) {
    T* pp = const_cast<T*>((T*)p);
    delete pp;
  }

public:
  const void* Get(const ResID ID);
  void  Put(const ResID ID, const void*, FreeResourceFunct); // FreeResourceFunct is NULL for non-unloadable or batched
  template<class T>
  T* PutGeneric(const ResID ID, T* p) { // will simply delete object on clear
    Put(ID, p, &ResourcesPool::GenericFreeResource<T>);
    return p;
  }
  void  Clear();
  ResourcesPool(u32 umax);
  ~ResourcesPool();

  static const char* GetNameFromID(const ResID ID);
  static ResID       GetIDFromName(const char*);
  static const ResID s_invalidID = ResID(~0);

private:
  struct Resource {
    Resource()
      : ID(s_invalidID),data(NULL), freefun(NULL){}
    Resource(ResID ID, const void* data, FreeResourceFunct freefun)
      : ID(ID), data(data), freefun(freefun){}
    ResID ID;
    const void* data;
    FreeResourceFunct freefun;
  };
  Resource* m_resources;
  u32       m_count;
  const u32 m_max;
};
NAMESPACE_END(res)
#endif // res_base_h__