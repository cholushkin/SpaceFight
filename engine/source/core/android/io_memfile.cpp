#include "io_memfile.h"
#include "core/common/com_misc.h"
#include "core/math/mt_base.h"

extern char* getFileContent(const char* pszPath, u32& sizeOfFile);

namespace io
{
  MemoryFile::MemoryFile(char* data, u32 size)
    :  m_data(data)
    ,  m_size(size)
    ,  m_pos(0) {}

  MemoryFile::~MemoryFile()
  {
    SAFE_DELETE_ARRAY(m_data);
    m_size = 0;
    m_pos  = 0;
  }

  IFile* MemoryFile::OpenFile( const char* szFile, bool bRead, bool bIsResourceFile )
  {
    u32 size;
    char* buff = getFileContent(szFile, size);
    return NULL != buff ? new MemoryFile(buff, size) : NULL;
  }

  bool MemoryFile::IsEOF() const
  {
    return m_size <= m_pos + 1;
  }

  bool MemoryFile::Seek( u32 dist )
  {
    if(m_pos + dist >= m_size)
      return false;
    m_pos += dist;
    return true;
  }

  u32 MemoryFile::Size() const
  {
    return m_size;
  }

  bool MemoryFile::Read( char* buf, u32 n )
  {
    if(!buf)
      return false;
    const u32 to_read = mt::Min(m_size - m_pos, n);
    memcpy(buf, &m_data[m_pos], to_read);
    m_pos += to_read;
    return n == to_read;
  }

  bool MemoryFile::Write( const char* p, u32 n )
  {
    EALWAYS_ASSERT("thou should not write to memory file");
    return false;
  }

}
