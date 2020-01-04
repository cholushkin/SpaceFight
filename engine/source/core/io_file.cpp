#include "io_file.h"
#include "platform.h"
namespace io
{

  extern "C" FILE* open_file(const char* szFile, bool bRead, bool bIsResourceFile);

  FileImpl::FileImpl(FILE* file)
    :  m_pFile(file)
  {}

  FileImpl::~FileImpl()
  {
    EASSERT(m_pFile);
    fclose(m_pFile);
    m_pFile = NULL;
  }

  IFile* FileImpl::OpenFile( const char* szFile, bool bRead, bool bIsResourceFile )
  {
    FILE* f = open_file(szFile, bRead, bIsResourceFile);
    return NULL != f ? new FileImpl(f) : NULL;
  }

  bool FileImpl::IsEOF() const
  {
    EASSERT(m_pFile);
    return 0 != feof(m_pFile);
  }

  bool FileImpl::Seek( u32 dist )
  {
    EASSERT(m_pFile);
    int res = fseek(m_pFile, dist, SEEK_CUR);
    EASSERT(0 == res);
    return 0 == res;
  }

  u32 FileImpl::Size() const
  {
    EASSERT(m_pFile);
    const long old_pos = ftell(m_pFile);
    int res = fseek(m_pFile, 0, SEEK_END);
    EASSERT(0 == res);
    const long pos = ftell(m_pFile);
    fseek(m_pFile, old_pos, SEEK_SET);
    return pos;
  }

  bool FileImpl::Read( char* p, u32 n )
  {
    EASSERT(m_pFile);
    const u32 read = fread(p,1,n,m_pFile);
    return n == read;
  }

  bool FileImpl::Write( const char* p, u32 n )
  {
    EASSERT(m_pFile);
    const u32 written = fwrite(p,1,n,m_pFile);
    return n == written;
  }

} // namespace io
