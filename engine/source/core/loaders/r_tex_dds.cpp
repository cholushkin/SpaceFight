#include "r_tex_dds.h"

NAMESPACE_BEGIN(dds)

#define DDS_MAGIC ('D'|('D'<<8)|('S'<<16)|(' '<<24))
#define DDS_DXT1  ('D'|('X'<<8)|('T'<<16)|('1'<<24))
#define DDS_DXT3  ('D'|('X'<<8)|('T'<<16)|('3'<<24))
#define DDS_DXT5  ('D'|('X'<<8)|('T'<<16)|('5'<<24))
#define DDS_ATCA  ('A'|('T'<<8)|('C'<<16)|('A'<<24))
#define DDS_ATCI  ('A'|('T'<<8)|('C'<<16)|('I'<<24))

// Decodes a DXT color block
void decodeColorBlock(u8 *dest, const int w, const int h, const int xOff, const int yOff, const DDSFormat format, const u8 *src){
  u8 colors[4][3];

  u16 c0 = *(u16 *) src;
  u16 c1 = *(u16 *) (src + 2);

  // Extract the two stored colors
  colors[0][0] = ((c0 >> 11) & 0x1F) << 3;
  colors[0][1] = ((c0 >>  5) & 0x3F) << 2;
  colors[0][2] =  (c0        & 0x1F) << 3;

  colors[1][0] = ((c1 >> 11) & 0x1F) << 3;
  colors[1][1] = ((c1 >>  5) & 0x3F) << 2;
  colors[1][2] =  (c1        & 0x1F) << 3;

  // Compute the other two colors
  if (c0 > c1 || format == FORMAT_DXT5)
  {
    for (int i = 0; i < 3; i++)
    {
      colors[2][i] = (2 * colors[0][i] +     colors[1][i] + 1) / 3;
      colors[3][i] = (    colors[0][i] + 2 * colors[1][i] + 1) / 3;
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      colors[2][i] = (colors[0][i] + colors[1][i] + 1) >> 1;
      colors[3][i] = 0;
    }
  }

  src += 4;
  for (int y = 0; y < h; y++)
  {
    u8 *dst = dest + yOff * y;
    u32 indexes = src[y];
    for (int x = 0; x < w; x++)
    {
      u32 index = indexes & 0x3;
      dst[0] = colors[index][0];
      dst[1] = colors[index][1];
      dst[2] = colors[index][2];
      indexes >>= 2;

      dst += xOff;
    }
  }
}

// Decode a DXT3 alpha block
void decodeDXT3AlphaBlock(u8 *dest, const int w, const int h, const int xOff, const int yOff, const u8 *src){
  for (int y = 0; y < h; y++)
  {
    u8 *dst = dest + yOff * y;
    u32 alpha = ((u16 *) src)[y];
    for (int x = 0; x < w; x++)
    {
      *dst = (alpha & 0xF) * 17;
      alpha >>= 4;
      dst += xOff;
    }
  }
}

// Decode a DXT5 alpha block / 3Dc channel block
void decodeDXT5AlphaBlock(u8 *dest, const int w, const int h, const int xOff, const int yOff, const u8 *src)
{
  u8 a0 = src[0];
  u8 a1 = src[1];
  u64 alpha = (*(u64 *) src) >> 16;

  for (int y = 0; y < h; y++)
  {
    u8 *dst = dest + yOff * y;
    for (int x = 0; x < w; x++)
    {
      int k = ((u32) alpha) & 0x7;

      if (k == 0)
        *dst = a0;
      else if (k == 1)
        *dst = a1;
      else if (a0 > a1)
        *dst = (u8)(((8 - k) * a0 + (k - 1) * a1) / 7);
      else if (k >= 6)
        *dst = (k == 6)? 0 : 255;
      else 
        *dst = (u8)(((6 - k) * a0 + (k - 1) * a1) / 5);
      alpha >>= 3;

      dst += xOff;
    }
    if (w < 4) alpha >>= (3 * (4 - w));
  }
}

DDSFormat getFormat(const DDS_Header_t& header)
{
  if(DDS_DXT1 == header.pfFourCC)
    return FORMAT_DXT1;
  if(DDS_DXT3 == header.pfFourCC)
    return FORMAT_DXT3;
  else if(DDS_DXT5 == header.pfFourCC)
    return FORMAT_DXT5;
  else if(DDS_ATCA == header.pfFourCC)
    return FORMAT_ATCA;
  else if(DDS_ATCI == header.pfFourCC)
    return FORMAT_ATCI;
  return FORMAT_MAX;
}

bool decodeCompressedImage(u8 *dest, const u8 *src, const DDS_Header_t& header)
{
  const int width  = header.Width;
  const int height = header.Height;
  
  DDSFormat format = FORMAT_MAX;

  if(DDS_DXT1 == header.pfFourCC)
    format = FORMAT_DXT1;
  if(DDS_DXT3 == header.pfFourCC)
    format = FORMAT_DXT3;
  else if(DDS_DXT5 == header.pfFourCC)
    format = FORMAT_DXT5;

  if(FORMAT_MAX == format)
    return false;

  int sx = (width  < 4)? width  : 4;
  int sy = (height < 4)? height : 4;

  int nChannels = 4;//getChannelCount(format);
  for (int y = 0; y < height; y += 4)
  {
    for (int x = 0; x < width; x += 4)
    {
      u8 *dst = dest + (y * width + x) * nChannels;
      if (format == FORMAT_DXT3)
      {
        decodeDXT3AlphaBlock(dst + 3, sx, sy, 4, width * 4, src);
        src += 8;
      }
      else if (format == FORMAT_DXT5)
      {
        decodeDXT5AlphaBlock(dst + 3, sx, sy, 4, width * 4, src);
        src += 8;
      }
      if (format <= FORMAT_DXT5)
      {
        decodeColorBlock(dst, sx, sy, nChannels, width * nChannels, format, src);
        src += 8;
      }
    }
  }
  return true;
}

NAMESPACE_END(dds)