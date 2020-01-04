#include "ext/mathalgs/mt_rle.h"
#include <string>

NAMESPACE_BEGIN(mt)

int RLE_Decode ( const u8* InBuffer, int nInSize, u8* OutBuffer ) {
  const u8* pOutStart = OutBuffer;
  while (nInSize-- > 0) {
    const u8 byData = *InBuffer++ ; // read byte and move ptr to next
    if ( (byData & 0xC0) == 0xC0 ) {
      const u8 cNum = byData & 0x3F; // repeat current byte Num
      ::memset (OutBuffer, *InBuffer++, cNum) ; // memset func will check "Num" =? 0
      OutBuffer += cNum;
      --nInSize;
    }
    else
      *OutBuffer++ = byData ;
  }
  return int(OutBuffer - pOutStart);
}

int RLE_Encode ( const u8* InBuffer, int nInSize, u8* OutBuffer ) {
  const u8* pOutStart = OutBuffer;
  while (nInSize-- > 0) {
    u8 cCount = 1 ;
    const u8 byData = *InBuffer++;
    while ( (cCount < 0x3F) && (nInSize != 0) )
      if (*InBuffer != byData)
        break;
      else {
        ++cCount;
        ++InBuffer;
        --nInSize;
      }

      if (cCount == 1) {
        if ( (byData & 0xC0) == 0xC0 ) { // Data >= 0xC0
          *OutBuffer++ = 0xC1;
          *OutBuffer++ = byData;
        }
        else
          *OutBuffer++ = byData;
      }
      else {
        *OutBuffer++ = 0xC0 | cCount;
        *OutBuffer++ = byData;
      }
  }
  return int(OutBuffer - pOutStart);
}

NAMESPACE_END(mt)