#ifndef r_tex_pvr_h__
#define r_tex_pvr_h__

// PVR textures
#define PVR_TEXTURE_FLAG_TYPE_MASK  0xff

//Compressed pixel formats
enum EPVRTPixelFormat
{
  ePVRTPF_PVRTCI_2bpp_RGB,
  ePVRTPF_PVRTCI_2bpp_RGBA,
  ePVRTPF_PVRTCI_4bpp_RGB,
  ePVRTPF_PVRTCI_4bpp_RGBA,
  ePVRTPF_PVRTCII_2bpp,
  ePVRTPF_PVRTCII_4bpp,
  ePVRTPF_ETC1,
  ePVRTPF_DXT1,
  ePVRTPF_DXT2,
  ePVRTPF_DXT3,
  ePVRTPF_DXT4,
  ePVRTPF_DXT5,
  
  //These formats are identical to some DXT formats.
  ePVRTPF_BC1 = ePVRTPF_DXT1,
  ePVRTPF_BC2 = ePVRTPF_DXT3,
  ePVRTPF_BC3 = ePVRTPF_DXT5,
  
  //These are currently unsupported:
  ePVRTPF_BC4,
  ePVRTPF_BC5,
  ePVRTPF_BC6,
  ePVRTPF_BC7,
  
  //These are supported
  ePVRTPF_UYVY,
  ePVRTPF_YUY2,
  ePVRTPF_BW1bpp,
  ePVRTPF_SharedExponentR9G9B9E5,
  ePVRTPF_RGBG8888,
  ePVRTPF_GRGB8888,
  ePVRTPF_ETC2_RGB,
  ePVRTPF_ETC2_RGBA,
  ePVRTPF_ETC2_RGB_A1,
  ePVRTPF_EAC_R11,
  ePVRTPF_EAC_RG11,
  
  //Invalid value
  ePVRTPF_NumCompressedPFs
};

typedef uint32_t PVRTuint32;
typedef uint64_t PVRTuint64;

static const uint32_t gPVRTexIdentifier = 0x03525650; // 'P''V''R'3

#pragma pack(push,4)
struct PVRTextureHeaderV3
{
  PVRTuint32  u32Version;     //Version of the file header, used to identify it.
  PVRTuint32  u32Flags;     //Various format flags.
  PVRTuint64  u64PixelFormat;   //The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
  PVRTuint32  u32ColourSpace;   //The Colour Space of the texture, currently either linear RGB or sRGB.
  PVRTuint32  u32ChannelType;   //Variable type that the channel is stored in. Supports signed/unsigned int/short/byte or float for now.
  PVRTuint32  u32Height;      //Height of the texture.
  PVRTuint32  u32Width;     //Width of the texture.
  PVRTuint32  u32Depth;     //Depth of the texture. (Z-slices)
  PVRTuint32  u32NumSurfaces;   //Number of members in a Texture Array.
  PVRTuint32  u32NumFaces;    //Number of faces in a Cube Map. Maybe be a value other than 6.
  PVRTuint32  u32MIPMapCount;   //Number of MIP Maps in the texture - NB: Includes top level.
  PVRTuint32  u32MetaDataSize;  //Size of the accompanying meta data.
};
#pragma pack(pop)
#define PVRTEX3_HEADERSIZE 52

#endif // r_tex_pvr_h__
