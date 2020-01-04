#ifndef io_memfile_h__
#define io_memfile_h__

 
#include "core/common/com_types.h"
#include "core/io/io_base.h"
 

// android impl

namespace io
{

  class MemoryFile
    : public IFile
  {
  public:
    virtual ~MemoryFile();
    static IFile* OpenFile( const char* szFile, bool bRead, bool bIsResourceFile );
    virtual bool  IsEOF() const;
    virtual bool  Seek(u32 dist);
    virtual u32   Size() const;
    virtual bool  Read (      char* p, u32 n);
    virtual bool  Write(const char* p, u32 n);
  private:
    MemoryFile(char* data, u32 size);
    char* m_data;
    u32   m_size;
    u32   m_pos;
  };

} // namespace io
 
#endif // io_memfile_h__
