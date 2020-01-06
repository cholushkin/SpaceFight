#ifndef uidgenerator_h__
#define uidgenerator_h__

class UidGenerator
{
public:
  UidGenerator()
  :  mLastUid(0)
  ,  mLastReuseIndex(-1)
  {
  }

  u32 getUid()
  {
    if(mLastReuseIndex!=-1)
      return mFreeUidPool[mLastReuseIndex--];
    return mLastUid++;
  }

  void freeUid(const u32& freeUid)
  {
    EASSERT(mLastReuseIndex < (i32)sMAX_REUSE_COUNT );
    mFreeUidPool[++mLastReuseIndex]=freeUid;
  }

  void clear()
  {
    mLastReuseIndex = -1;
    mLastUid = 0;
  }
private:
  static const u32 sMAX_REUSE_COUNT = 1024;
  u32 mLastUid;
  i32 mLastReuseIndex;
  u32 mFreeUidPool[sMAX_REUSE_COUNT];
};


#endif // uidgenerator_h__