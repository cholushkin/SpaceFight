#include "ext/mathalgs/mt_crc32.h"

NAMESPACE_BEGIN(mt)

static bool isTableInited = false;
static u32 crcTable[256];

u32 Crc32(  u8* dataBuf, u32 bufSize ) {
  u32 crc;

  if ( !isTableInited ) {
		for ( u32 i = 0; i < 256; i++ ) {
			crc = i;
			for (int j = 0; j < 8; j++ )
				crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
			crcTable[i] = crc;
		}
		isTableInited = true;
	}	

	crc = 0xFFFFFFFFUL;

	while (bufSize--) 
		crc = crcTable[(crc ^ *dataBuf++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}

NAMESPACE_END(mt)