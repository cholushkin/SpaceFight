#include "ext/strings/str_stringtable.h"
#include "core/io/io_base.h"
#include "platform.h"

NAMESPACE_BEGIN(str)

const wchar_t * const StringTable::GetString(u32 id) const
{
  EASSERT(m_nEtries > id);
  if(m_nEtries > id)
    return &m_strings[m_offsets[id]];
  static const wchar_t empty[] = L"";
  return empty;
}

const wchar_t * const StringTable::GetString(const char* key) const
{
  return GetString(FindKey(key));
}

u32 StringTable::FindKey(const char* key) const
{
  for(u32 i = 0; m_nEtries != i; ++i)
    if(0 == strcmp(key, m_keys[i]))
      return i;
  return m_nEtries;
}

bool StringTable::Load (const char* filename, const char* dictfile)
{
  // todo: refactor me
  Clear();
  io::FileStream fs;
  if(!fs.Open(filename, true, true))
    return false;
  fs.Read32(m_nEtries);
  u32 size;
  fs.Read32(size);
  m_offsets = new u32[m_nEtries];
  if(!fs.Read((char*)m_offsets, m_nEtries * sizeof(u32)))
  {
    Clear();
    return false;
  }

  m_strings = new wchar_t[size];
  if(!fs.Read((char*)m_strings, size * sizeof(wchar_t)))
  {
    Clear();
    return false;
  }
  if(!dictfile)
    return true;
  if(!fs.Open(dictfile, true, true))
  {
    Clear();
    return false;
  }
  fs.Read32(size);
  EASSERT(size == m_nEtries); // should match
  if(size != m_nEtries)
  {
    Clear();
    return false;
  }
  fs.Read32(size); // keys string length
  m_dict = new char[size];
  if(!fs.Read((char*)m_dict, size * sizeof(char)))
  {
    Clear();
    return false;
  }
  // todo: refactor me (this code is copy-pasted in ASprite and SpriteSheet)
  m_keys = new const char*[m_nEtries];
  size_t idx = 0;
  char* pn = &m_dict[0];
  m_keys[idx++] = pn;
  for(; m_nEtries != idx;)
  {
    while(*(pn++));
    m_keys[idx++] = pn;
  }
  return true;
}

void StringTable::Clear()
{
  SAFE_DELETE_ARRAY(m_strings);
  SAFE_DELETE_ARRAY(m_offsets);
  SAFE_DELETE_ARRAY(m_dict);
  SAFE_DELETE_ARRAY(m_keys);
  m_nEtries = 0;
}

StringTable::~StringTable()
{
  Clear();
}

NAMESPACE_END(str)