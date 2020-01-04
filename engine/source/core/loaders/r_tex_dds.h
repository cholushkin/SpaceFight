#ifndef r_tex_dds_h__
#define r_tex_dds_h__

#include "core/common/com_types.h"
#include "core/common/com_misc.h"

NAMESPACE_BEGIN(dds)

typedef struct
{
  u32 Magic;
  u32 Size;
  u32 Flags;
  u32 Height;
  u32 Width;
  u32 PitchLinearSize;
  u32 Depth;
  u32 MipMapCount;
  u32 Reserved1[11];
  u32 pfSize;
  u32 pfFlags;
  u32 pfFourCC;
  u32 pfRGBBitCount;
  u32 pfRMask;
  u32 pfGMask;
  u32 pfBMask;
  u32 pfAMask;
  u32 Caps1;
  u32 Caps2;
  u32 Reserved2[3];
} DDS_Header_t;

enum DDSFormat
{
  FORMAT_DXT1,
  FORMAT_DXT3,
  FORMAT_DXT5,
  FORMAT_ATCI, // decompression is not supported
  FORMAT_ATCA, // decompression is not supported
  FORMAT_MAX
};

bool      decodeCompressedImage(u8 *dest, const u8 *src, const DDS_Header_t& header);
DDSFormat getFormat(const DDS_Header_t& header);

NAMESPACE_END(dds)

#endif // r_tex_dds_h__
