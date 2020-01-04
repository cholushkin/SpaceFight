#ifndef colors_h__
#define colors_h__

#include "core/math/mt_base.h"

NAMESPACE_BEGIN(mt)

#define ARGB(a,r,g,b) ((u32(a)<<24) + (u32(b)<<16) + (u32(g)<<8) + u32(r))
#define GETA(col)   ((col)>>24)
#define GETB(col)   (((col)>>16) & 0xFF)
#define GETG(col)   (((col)>>8) & 0xFF)
#define GETR(col)   ((col) & 0xFF)
#define SETA(col,a) (((col) & 0x00FFFFFF) + (u32(a)<<24))
#define SETB(col,r) (((col) & 0xFF00FFFF) + (u32(r)<<16))
#define SETG(col,g) (((col) & 0xFFFF00FF) + (u32(g)<<8))
#define SETR(col,b) (((col) & 0xFFFFFF00) +  u32(b))

inline u32 make_color(float opacity, u8 r, u8 g, u8 b) {
  return ARGB(u8(0xFF * opacity), r, g, b);
}

inline u32 make_color(float opacity, u32 argb) {
  return SETA(argb, opacity * (f32)GETA(argb));
}

// ----- color constants (gl color format - abgr)
const u32  COLOR_ALICEBLUE               = 0xFFFFF8F0;
const u32  COLOR_ANTIQUEWHITE            = 0xFFD7EBFA;
const u32  COLOR_AQUA                    = 0xFFFFFF00; 
const u32  COLOR_AQUAMARINE              = 0xFFD4FF7F;
const u32  COLOR_AZURE                   = 0xFFFFFFF0;
const u32  COLOR_BEIGE                   = 0xFFDCF5F5;
const u32  COLOR_BISQUE                  = 0xFFC4E4FF;
const u32  COLOR_BLACK                   = 0xFF000000;
const u32  COLOR_BLANCHEDALMOND          = 0xFFCDEBFF;
const u32  COLOR_BLUE                    = 0xFFFFFF00;
const u32  COLOR_DBLUE                   = 0xFFFF0000;
const u32  COLOR_BLUEVIOLET              = 0xFFE22B8A;
const u32  COLOR_BROWN                   = 0xFF2A2AA5;
const u32  COLOR_BURLYWOOD               = 0xFF8788DE;
const u32  COLOR_CADETBLUE               = 0xFFA09E5F;
const u32  COLOR_CHOCOLATE               = 0xFF1E69D2;
const u32  COLOR_CORAL                   = 0xFF507FFF;
const u32  COLOR_CORNFLOWERBLUE          = 0xFFED9564;
const u32  COLOR_CORNSILK                = 0xFFDCF8FF;
const u32  COLOR_CRIMSON                 = 0xFF3C14DC;
const u32  COLOR_CYAN                    = 0xFFFFFF00;
const u32  COLOR_DARKBLUE                = 0xFF8B0000;
const u32  COLOR_DARKCYAN                = 0xFF8B8B00;
const u32  COLOR_DARKGOLDENROD           = 0xFF0B86B8;
const u32  COLOR_DARKGRAY                = 0xFFA9A9A9;
const u32  COLOR_DARKGREEN               = 0xFF006400;
const u32  COLOR_DARKKHAKI               = 0xFF6DB7BD;
const u32  COLOR_DARKMAGENTA             = 0xFF8B008B;
const u32  COLOR_DARKOLIVEGREEN          = 0xFF2F6B55;
const u32  COLOR_DARKORANGE              = 0xFF008CFF;
const u32  COLOR_DARKORCHID              = 0xFFCC3299;
const u32  COLOR_DARKRED                 = 0xFF00008B;
const u32  COLOR_DARKSALMON              = 0xFF7A96E9;
const u32  COLOR_DARKSEAGREEN            = 0xFF8FBC8F;
const u32  COLOR_DARKSLATEBLUE           = 0xFF8B3D48;
const u32  COLOR_DARKSLATEGRAY           = 0xFF4F4F2F;
const u32  COLOR_DARKTURQUOISE           = 0xFFD1CE00;
const u32  COLOR_DARKVIOLET              = 0xFFD30094;
const u32  COLOR_DEEPPINK                = 0xFF9314FF;
const u32  COLOR_DEEPSKYBLUE             = 0xFFFFBF00;
const u32  COLOR_DIMGRAY                 = 0xFF696969;
const u32  COLOR_DODGERBLUE              = 0xFFFF901E;
const u32  COLOR_FIREBRICK               = 0xFF2222B2;
const u32  COLOR_FLORALWHITE             = 0xFFF0FAFF;
const u32  COLOR_FORESTGREEN             = 0xFF228B22;
const u32  COLOR_FUCHSIA                 = 0xFFFF00FF;
const u32  COLOR_GAINSBORO               = 0xFFDCDCDC;
const u32  COLOR_GHOSTWHITE              = 0xFFFFF8F8;
const u32  COLOR_GOLD                    = 0xFF00D7FF;
const u32  COLOR_GOLDENROD               = 0xFF20A5DA;
const u32  COLOR_GRAY                    = 0xFF808080;
const u32  COLOR_GREEN                   = 0xFF008000;
const u32  COLOR_GREENYELLOW             = 0xFF2FFFAD;
const u32  COLOR_HONEYDEW                = 0xFFF0FFF0;
const u32  COLOR_HOTPINK                 = 0xFFB469FF;
const u32  COLOR_INDIANRED               = 0xFF5C5CCD;
const u32  COLOR_INDIGO                  = 0xFF82004B;
const u32  COLOR_IVORY                   = 0xFFF0FFFF;
const u32  COLOR_KHAKI                   = 0xFF8CE6F0;
const u32  COLOR_LAVENDER                = 0xFFFAE6E6;
const u32  COLOR_LAVENDERBLUSH           = 0xFFF5F0FF;
const u32  COLOR_LEMONCHIFFON            = 0xFFCDFAFF;
const u32  COLOR_LIGHTBLUE               = 0xFFE6D8AD;
const u32  COLOR_LIGHTCORAL              = 0xFF8080F0;
const u32  COLOR_LIGHTCYAN               = 0xFFFFFFE0;
const u32  COLOR_LIGHTGOLDENROD          = 0xFFD2FAFA;
const u32  COLOR_LIGHTGREEN              = 0xFF90EE90;
const u32  COLOR_LIGHTPINK               = 0xFFC1B6FF;
const u32  COLOR_LIGHTSALMON             = 0xFF7AA0FF;
const u32  COLOR_LIGHTSEAGREEN           = 0xFFAAB220;
const u32  COLOR_LIGHTSKYBLUE            = 0xFFFACE87;
const u32  COLOR_LIGHTSLATEGRAY          = 0xFF998877;
const u32  COLOR_LIGHTSTEELBLUE          = 0xFFDEC4B0;
const u32  COLOR_LIGHTYELLOW             = 0xFFE0FFFF;
const u32  COLOR_LIME                    = 0xFF00FF00;
const u32  COLOR_LIMEGREEN               = 0xFF32CD32;
const u32  COLOR_LINEN                   = 0xFFF6F0FA;
const u32  COLOR_MAGENTA                 = 0xFFFF00FF;
const u32  COLOR_MAROON                  = 0xFF000080;
const u32  COLOR_MEDIUMAQUAMARINE        = 0xFFAACD66;
const u32  COLOR_MEDIUMBLUE              = 0xFFCD0000;
const u32  COLOR_MEDIUMORCHID            = 0xFFD355BA;
const u32  COLOR_MEDIUMPURPLE            = 0xFFDB7093;
const u32  COLOR_MEDIUMSEAGREEN          = 0xFF71B33C;
const u32  COLOR_MEDIUMSLATEBLUE         = 0xFFEE687B;
const u32  COLOR_MEDIUMSPRINGGREEN       = 0xFF9AFA00;
const u32  COLOR_MEDIUMTURQUOISE         = 0xFFCCD148;
const u32  COLOR_MEDIUMVIOLETRED         = 0xFF8515C7;
const u32  COLOR_MIDNIGHTBLUE            = 0xFF701919;
const u32  COLOR_MINTCREAM               = 0xFFFAFFF5;
const u32  COLOR_MISTYROSE               = 0xFFE1E4FF;
const u32  COLOR_MOCCASIN                = 0xFFB5E4FF;
const u32  COLOR_NAVAJOWHITE             = 0xFFADDEFF;
const u32  COLOR_NAVY                    = 0xFF800000;
const u32  COLOR_OLDLACE                 = 0xFFE6F5FD;
const u32  COLOR_OLIVE                   = 0xFF008080;
const u32  COLOR_OLIVEDRAB               = 0xFF238E6B;
const u32  COLOR_ORANGE                  = 0xFF00A5FF;
const u32  COLOR_ORANGERED               = 0xFF0045FF;
const u32  COLOR_ORCHID                  = 0xFFD670DA;
const u32  COLOR_PALEGOLDENROD           = 0xFFAAE8EE;
const u32  COLOR_PALEGREEN               = 0xFF98FB98;
const u32  COLOR_PALETURQUOISE           = 0xFFEEEEAF;
const u32  COLOR_PALEVIOLETRED           = 0xFF9370DB;
const u32  COLOR_PAPAYAWHIP              = 0xFFD5EFFF;
const u32  COLOR_PEACHPUFF               = 0xFFB9DAFF;
const u32  COLOR_PERU                    = 0xFF3F85CD;
const u32  COLOR_PINK                    = 0xFFCBC0FF;
const u32  COLOR_PLUM                    = 0xFFDDA0DD;
const u32  COLOR_POWDERBLUE              = 0xFFE6E0B0;
const u32  COLOR_PURPLE                  = 0xFF800080;
const u32  COLOR_RED                     = 0xFF0000FF;
const u32  COLOR_ROSYBROWN               = 0xFF8F8FBC;
const u32  COLOR_ROYALBLUE               = 0xFFE16941;
const u32  COLOR_SALMON                  = 0xFF7280FA;
const u32  COLOR_SANDYBROWN              = 0xFF60A4F4;
const u32  COLOR_SEAGREEN                = 0xFF572B2E;
const u32  COLOR_SEASHELL                = 0xFFEEE5FF;
const u32  COLOR_SIENNA                  = 0xFF2D52A0;
const u32  COLOR_SILVER                  = 0xFFC0C0C0;
const u32  COLOR_SKYBLUE                 = 0xFFEBCE87;
const u32  COLOR_SLATEBLUE               = 0xFFCD5A6A;
const u32  COLOR_SLATEGRAY               = 0xFF908070;
const u32  COLOR_SNOW                    = 0xFFFAFAFF;
const u32  COLOR_SPRINGGREEN             = 0xFF7FFF00;
const u32  COLOR_STEELBLUE               = 0xFFB48246;
const u32  COLOR_TAN                     = 0xFF8CB4D2;
const u32  COLOR_TEAL                    = 0xFF808000;
const u32  COLOR_THISTLE                 = 0xFFD8BFD8;
const u32  COLOR_TOMATO                  = 0xFF4763FF;
const u32  COLOR_TURQUOISE               = 0xFFD0E040;
const u32  COLOR_VIOLET                  = 0xFFEE82EE;
const u32  COLOR_WHEAT                   = 0xFFB3DEF5;
const u32  COLOR_WHITE                   = 0xFFFFFFFF;
const u32  COLOR_WHITESMOKE              = 0xFFF5F5F5;
const u32  COLOR_YELLOW                  = 0xFF00FFFF;
const u32  COLOR_YELLOWGREEN             = 0xFF9ACD32;

struct HSVColor
{
  f32 fHue;         // [0.f..360.f]
  f32 fSaturation;  // [0.f..1.f]
  f32 fValue;       // [0.f..1.f]

  HSVColor()
    : fHue(0)
    , fSaturation(0)
    , fValue(0) {}

  HSVColor(f32 fH, f32 fS, f32 fV)
    : fHue(fH)
    , fSaturation(fS)
    , fValue(fV) {}

  u32 toU32() const;
  u32 toU32(f32 /*[0..1]*/ a) const;
};

struct RGBColor
{
  f32 fR;  // [0.f..1.f]
  f32 fG;  // [0.f..1.f]
  f32 fB;  // [0.f..1.f]
  RGBColor()
    : fR(0), fG(0), fB(0) {}
  explicit RGBColor(const u32 abgr)
  {
    const f32 rc = 1.0f / 255.0f;
    fR = GETR(abgr) * rc;
    fG = GETG(abgr) * rc;
    fB = GETB(abgr) * rc;
  }
  explicit RGBColor(const HSVColor&);

  u32 toU32() const{
    return ARGB(255, flt_to_byte(fR * 255.0f), flt_to_byte(fG * 255.0f), flt_to_byte(fB * 255.0f));
  }

  u32 toU32(f32 /*[0..1]*/ a) const {
    return SETA(toU32(), flt_to_byte(a * 255.0f));
  }
};

inline void HSV2RGB( const HSVColor& i_HSV, RGBColor& o_RGB )
{
  int h = ((int)i_HSV.fHue / 60) % 6;
  f32 f = i_HSV.fHue / 60.f - (f32)((int)i_HSV.fHue / 60);
  f32 p = i_HSV.fValue * (1 - i_HSV.fSaturation);
  f32 q = i_HSV.fValue * (1 - f * i_HSV.fSaturation);
  f32 t = i_HSV.fValue * (1 - (1 - f) * i_HSV.fSaturation);
  if ( h == 0 )  
  {
    o_RGB.fR = i_HSV.fValue;
    o_RGB.fG = t;
    o_RGB.fB = p;  
  } 
  else if ( h == 1 )
  { 
    o_RGB.fR = q;
    o_RGB.fG = i_HSV.fValue; 
    o_RGB.fB = p; 
  }
  else if ( h == 2 )
  {
    o_RGB.fR = p;
    o_RGB.fG = i_HSV.fValue;
    o_RGB.fB = t;
  }
  else if ( h == 3 )
  {
    o_RGB.fR = p;
    o_RGB.fG = q;
    o_RGB.fB = i_HSV.fValue;
  }
  else if ( h == 4 )
  {
    o_RGB.fR = t;
    o_RGB.fG = p;
    o_RGB.fB = i_HSV.fValue;
  }
  else if ( h == 5 )
  {
    o_RGB.fR = i_HSV.fValue;
    o_RGB.fG = p;
    o_RGB.fB = q;
  }
  else
    EALWAYS_ASSERT("");
}

inline RGBColor::RGBColor(const HSVColor& rHSV)
{
  HSV2RGB(rHSV, *this);
}

inline u32 HSVColor::toU32() const
{
  return RGBColor(*this).toU32();
}

inline u32 HSVColor::toU32(f32 /*[0..1]*/ a) const
{
  return RGBColor(*this).toU32(a);
}

NAMESPACE_END(mt)
#endif // colors_h__
