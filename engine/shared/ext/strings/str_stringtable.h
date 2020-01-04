#ifndef str_stringtable_h__
#define str_stringtable_h__

#include "core/common/com_misc.h"

NAMESPACE_BEGIN(str)

class StringTable
{
public:
  StringTable()
    : m_strings(NULL)
    , m_offsets(NULL)
    , m_dict(NULL)
    , m_keys(NULL)
    , m_nEtries(0)
  {}
  ~StringTable();
  const wchar_t * const GetString(      u32    id) const;
  const wchar_t * const GetString(const char* key) const;
  bool Load (const char* filename, const char* dictfile = NULL);
  void Clear();

private:
  u32 FindKey(const char* key) const; // return m_nEtries if not found

  wchar_t* m_strings;
  u32*     m_offsets;
  u32      m_nEtries;

  char*    m_dict;
  const char** m_keys;
};

NAMESPACE_END(str)

#endif // str_stringtable_h__
