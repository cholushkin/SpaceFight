#ifndef file_impl_h__
#define file_impl_h__
#include "core/common/com_types.h"
#include "core/io/io_base.h"
#include <stdio.h>

// stdio implementation

namespace io
{

  class FileImpl
    : public IFile
  {
  public:
    static IFile* OpenFile(const char* szFile, bool bRead, bool bIsResourceFile);

    ~FileImpl();

    virtual bool IsEOF() const;
    virtual bool Seek(u32 dist);
    virtual u32  Size() const;
    virtual bool Read (      char* p, u32 n);
    virtual bool Write(const char* p, u32 n);
  private:
    FileImpl(FILE*);
    FILE* m_pFile;  
  };

} // namespace io

#endif // file_impl_h__
