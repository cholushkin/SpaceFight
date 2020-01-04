#ifndef _io_base_h
#define _io_base_h

#include "core/common/com_misc.h"
#include <string>
NAMESPACE_BEGIN(io)

class IFile
{
public:
  virtual ~IFile(){}
  virtual bool IsEOF() const = 0;
  virtual bool Seek(u32 dist) = 0;
  virtual u32  Size() const = 0;
  virtual bool Read (      char* p, u32 n) = 0;
  virtual bool Write(const char* p, u32 n) = 0;
};

class FileStream
{
public:
  FileStream()
    : m_File(NULL){}
  ~FileStream();

  bool Open (const char* szFile, bool bRead, bool bIsResourceFile);
  void Close();

  bool Read32(u32& );
  bool Read16(u16& );
  bool Read8 (u8&  );
  bool Read32(i32& );
  bool Read16(i16& );
  bool Read8 (i8&  );
  bool ReadF (float&);
  bool Read  (char*, u32 n);

  bool Write32(u32);
  bool Write16(u16);
  bool Write8 (u8);
  bool WriteF (float);
  bool Write  (const char*, u32 n);

  bool isEOF() const;
  // seek
  bool Seek(u32 dist);
  // tell
  // flush
  u32 Size();
private:
  IFile*     m_File;
  FileStream(const FileStream&);
  FileStream& operator=(const FileStream&);
};

template<typename T>
bool Write( FileStream& f, T const& value )
{
  return f.Write( (char const*)(&value), sizeof(T) );
}

template<typename T>
bool Read( FileStream& f, T& value )
{
  return f.Read( (char*)(&value), sizeof(T) );
}

template<typename T>
bool Serialize( FileStream& f, bool store, T& value )
{
  return store ? Write(f, value) : Read(f, value);
}

template<typename T>
bool SerializeArray( FileStream& f, bool store, T* values, size_t size)
{
  bool ok = Serialize(f, store, size);
  for( size_t i = 0; ok && i < size; ++i )
    ok &= Serialize( f, store, values[i] );
  return ok;
}

template<typename T>
bool SerializePlaneArray( FileStream& f, bool store, T* values, size_t size)
{
  if( Serialize(f, store, size) )
    return store ? f.Write( (char*)values, size * sizeof(T) ) : f.Read( (char*)values, size * sizeof(T) );

  return false;
}

//////////////////////////////////////////////////////////////////////////
// Helper functions
inline bool WriteStr(FileStream& f, const char* str)
{
  if(!str)
    return f.Write32(0);
  u32 len = (u32)strlen(str)+1;
  if(!f.Write32(len))
    return false;
  return f.Write(str,len);
}

inline char* ReadStr(FileStream& f)
{
  u32 len(0);
  if(!f.Read32(len) || (0 == len))
    return NULL;
  char* str = new char[len];
  f.Read(str,len);
  return str;
}

// Array reading/writing
// brute force implementation

inline bool WriteArray32(FileStream& f, u32* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.Write32(pArr[i]))
      return false;
  return true;
}

inline bool WriteArray16(FileStream& f, u16* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.Write16(pArr[i]))
      return false;
  return true;
}

inline bool WriteArrayF(FileStream& f, float* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.WriteF(pArr[i]))
      return false;
  return true;
}

inline bool ReadArray32(FileStream& f, u32* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.Read32(pArr[i]))
      return false;
  return true;
}

inline bool ReadArray16(FileStream& f, u16* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.Read16(pArr[i]))
      return false;
  return true;
}

inline bool ReadArrayF(FileStream& f, float* pArr, size_t n)
{
  if(0 == n)
    return true;
  for(size_t i = 0; n != i; ++i)
    if(!f.ReadF(pArr[i]))
      return false;
  return true;
}

NAMESPACE_END(io)

#endif // _io_base_h
