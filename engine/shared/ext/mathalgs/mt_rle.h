#ifndef mt_rle_h__
#define mt_rle_h__

#include "core/common/com_misc.h"

NAMESPACE_BEGIN(mt)

int RLE_Decode ( const u8* InBuffer, i32 nInSize, u8* OutBuffer );
int RLE_Encode ( const u8* InBuffer, i32 nInSize, u8* OutBuffer );

NAMESPACE_END(mt)

#endif // mt_rle_h__
