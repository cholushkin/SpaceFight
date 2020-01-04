#include "core/res/res_base.h"
#include <string>
#include "platform.h"


NAMESPACE_BEGIN(res)

extern ResourceTableEntry m_gameresources[];
extern u32                m_ngameresources;

ResourcesPool::ResourcesPool(u32 umax)
: m_resources(NULL)
, m_count(0)
, m_max(umax) {
  m_resources = new Resource[umax];
}

ResourcesPool::~ResourcesPool() {
  Clear();
  SAFE_DELETE_ARRAY(m_resources);
}

const void* ResourcesPool::Get(const ResID ID) {
  for (u32 i = 0; m_count != i;++i)
    if(ID == m_resources[i].ID)
      return m_resources[i].data;
  return NULL;
}

void ResourcesPool::Put(ResID ID, const void* p, FreeResourceFunct f) {
  EASSERT(m_count < m_max);
  m_resources[m_count] = Resource(ID, p, f);
  ++m_count;
}

void ResourcesPool::Clear() {
  for (u32 i = 0; m_count != i;++i) {
    if(m_resources[i].freefun)
      m_resources[i].freefun(m_resources[i].data);
    m_resources[i] = Resource();
  }
  m_count = 0;
}

const char* ResourcesPool::GetNameFromID(const ResID ID) {
  for (u32 i = 0; m_ngameresources != i;++i) {
    if(m_gameresources[i].m_id == ID)
      return m_gameresources[i].m_name;
  }
  return NULL;
}

ResID ResourcesPool::GetIDFromName(const char* name) {
  for (u32 i = 0; m_ngameresources != i;++i) {
    if(0 == stricmp(m_gameresources[i].m_name, name))
      return m_gameresources[i].m_id;
  }
  return s_invalidID;
}


NAMESPACE_END(res)