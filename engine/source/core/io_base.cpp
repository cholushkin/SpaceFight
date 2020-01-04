#include "core/io/io_base.h"
#include "io_file.h"
#include "platform.h"

#ifdef ANDROID_PLATFORM
# include "android/io_memfile.h"
#endif

NAMESPACE_BEGIN(io)

#define LTL(X) X

FileStream::~FileStream()
{
  Close();
}

bool FileStream::Open(const char* szFile, bool bRead, bool bIsResourceFile)
{
  Close();
  if(bIsResourceFile && !bRead)
  {
    EALWAYS_ASSERT("One can not write to resource files");
    return false;
  }

  if(szFile)
  {
#ifdef ANDROID_PLATFORM
    if(bIsResourceFile)
      m_File = MemoryFile::OpenFile(szFile,bRead,bIsResourceFile);
    else
#endif
     m_File = FileImpl::OpenFile(szFile,bRead,bIsResourceFile);
  }
  return NULL != m_File;
}

void FileStream::Close()
{
  SAFE_DELETE(m_File);
}

bool FileStream::isEOF() const
{
  EASSERT(m_File);
  return m_File && m_File->IsEOF();
}

bool FileStream::Seek(u32 dist)
{
  EASSERT(m_File);
  if(!m_File)
    return false;
  return m_File->Seek(dist);
}

u32 FileStream::Size()
{
  EASSERT(m_File);
  return m_File ? m_File->Size() : 0;
}

bool FileStream::Read(char* p, u32 n)
{
  if(!m_File || !p)
    return false;
  return m_File->Read(p,n);
}

bool FileStream::Write(const char* p, u32 n)
{
  if(!m_File)
    return false;
  return m_File->Write(p,n);
}

bool FileStream::Read32(u32& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  LTL(u);
  return true;
}

bool FileStream::Read16(u16& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  LTL(u);
  return true;
}

bool FileStream::Read8(u8& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  return true;
}

bool FileStream::Read32(i32& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  LTL(u);
  return true;
}

bool FileStream::Read16(i16& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  LTL(u);
  return true;
}

bool FileStream::Read8(i8& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  return true;
}

bool FileStream::ReadF(float& u)
{
  if(!m_File)
    return false;
  if(!Read((char*)&u, sizeof(u)))
    return false;
  LTL(u);
  return true;
}

bool FileStream::Write32(u32 u)
{
  if(!m_File)
    return false;
  LTL(u);
  if(!Write((const char*)&u, sizeof(u)))
    return false;
  return true;
}

bool FileStream::Write16(u16 u)
{
  if(!m_File)
    return false;
  LTL(u);
  if(!Write((const char*)&u, sizeof(u)))
    return false;
  return true;
}

bool FileStream::Write8(u8 u)
{
  if(!m_File)
    return false;
  if(!Write((const char*)&u, sizeof(u)))
    return false;
  return true;;
}

bool FileStream::WriteF(float u)
{
  if(!m_File)
    return false;
  LTL(u);
  if(!Write((const char*)&u, sizeof(u)))
    return false;
  return true;
}


NAMESPACE_END(io)
