#ifndef r_primitives_h 
#define r_primitives_h 

#include "core/render/r_render.h"
#include "core/math/mt_base.h"
#include "ext/math/mt_colors.h"

NAMESPACE_BEGIN(r)

const mt::v2f g_def_uv    [4] = {mt::v2f( 0, 0),  mt::v2f( 1, 0),  mt::v2f( 1, 1),  mt::v2f( 0, 1)};
const u32     g_def_colors[4] = {mt::COLOR_WHITE, mt::COLOR_WHITE, mt::COLOR_WHITE, mt::COLOR_WHITE};

//////////////////////////////////////////////////////////////////////////
//// 2D

//////////////////////////////////////////////////////////////////////////
// Frame9

class Frame9
{
public:
  mt::v2f  m_tl;   // top left corner
  mt::v2f  m_br;   // buttom right corner
  Material m_mat;
  f32      m_opacity;

  Frame9()
    : m_opacity(1.0f){}

  void Draw(Render& r) const;
  static void Draw(Render& r, const mt::Rectf& target_rect, const mt::Rectf& uv_rect, const mt::Rectf& source_rect, const r::Material& mat, f32 opacity);
private:
};

//////////////////////////////////////////////////////////////////////////
// TileMap
class SheetSprite;

class TileMap
{
public:
  TileMap();
  ~TileMap();
  void Load(const SheetSprite*, u16 w, u16 h, u16* data); // will own data
  void Draw(Render& r, const mt::v2f& pos) const;
  void Draw(Render& r, const mt::v2f& pos, const mt::Rectf& clip) const;
private:
  const SheetSprite* m_spr;
  u16*      m_data;
  mt::v2u16 m_size;
};

//////////////////////////////////////////////////////////////////////////
// GridMesh

class GridMesh
{
  u32 m_nRows;
  u32 m_nCols;
  struct Vertex
  {
    mt::v2f v;
    mt::v2f uv;
  };
  Vertex* m_verts;

  mt::v2f m_Vs [2]; // tl, br
  mt::v2f m_UVs[2]; // tl, br
public:

  GridMesh()
    : m_verts(NULL)
    , m_nRows(2)
    , m_nCols(2)
  {
    m_UVs[0] = mt::v2f(0.0f, 0.0f);
    m_UVs[1] = mt::v2f(1.0f, 1.0f);

    m_Vs [0] = mt::v2f(0.0f, 0.0f);
    m_Vs [1] = mt::v2f(0.0f, 0.0f);
    RebuildGrid();
  }

  void SetUV(f32 uv0, f32 uv1, f32 uv2, f32 uv3)
  {
    m_UVs[0].x = uv0;
    m_UVs[0].y = uv1;
    m_UVs[1].x = uv2;
    m_UVs[1].y = uv3;
    RebuildGrid();
  }

  void SetDims(u32 dim_x, u32 dim_y)
  {
    m_nCols = mt::Max(dim_x, 2U);
    m_nRows = mt::Max(dim_y, 2U);
    RebuildGrid();
  }

  void SetSize(f32 w, f32 h)
  {
    m_Vs [0] = mt::v2f(0.0f, 0.0f);
    m_Vs [1] = mt::v2f(w, h);
    RebuildGrid();
  }

  void SetSize(const mt::v2f& tl, const mt::v2f& br)
  {
    m_Vs [0] = tl;
    m_Vs [1] = br;
    RebuildGrid();
  }

  ~GridMesh()
  {
    SAFE_DELETE_ARRAY(m_verts);
  }

  void RebuildGrid();

  void SetDisplacement  ( u32 col, u32 row, f32 dx,   f32 dy );
  void SetUVDisplacement( u32 col, u32 row, f32 uvdx, f32 uvdy);

  void Draw(Render& r, const Material& m, const mt::Matrix23f& mtr, const unsigned int color) const;
  void Draw(Render& r, const Material& m, const mt::v2f&       pos, const unsigned int color) const;

  u32 GetRows() const { return m_nRows; }
  u32 GetCols() const { return m_nCols; }
};


class WaterRipple
{
public:
  enum WRSide
  {
    WR_Top    = 1 << 0,
    WR_Right  = 1 << 1,
    WR_Bottom = 1 << 2,
    WR_Left   = 1 << 3,
    WR_All    = ( WR_Top | WR_Right | WR_Bottom | WR_Left),
  };

  WaterRipple()
    : m_freq(0)
    , m_diff(0)
    , m_amplitude_x(0)
    , m_amplitude_y(0)
    , m_fadeMode(0)
    , m_fixMode(0)
  {}

  WaterRipple(f32 freq, f32 diff, f32 amplitude_x, f32 amplitude_y, int fadeMode, int fixMode)
    : m_freq(freq)
    , m_diff(diff)
    , m_amplitude_x(amplitude_x)
    , m_amplitude_y(amplitude_y)
    , m_fadeMode(fadeMode)
    , m_fixMode(fixMode)
  {  }

  void Apply(GridMesh& mesh, f32 t) const
  {
    const u32 lastRow  = mesh.GetRows() - (( m_fixMode & WR_Bottom ) ? 1 : 0);
    const u32 lastCol  = mesh.GetCols() - (( m_fixMode & WR_Right )  ? 1 : 0);
    const f32 fRows   = static_cast<f32>( mesh.GetRows() );
    const f32 fCols   = static_cast<f32>( mesh.GetCols() );

    for( u32 r =  (m_fixMode & WR_Top)  ? 1 : 0; lastRow != r; ++r)
    {
      for(u32 c = (m_fixMode & WR_Left) ? 1 : 0; lastCol != c; ++c)  
      {
        const f32 fadeX = 
          m_fadeMode & WR_Left 
          ? ( c / fCols ) 
          : (  m_fadeMode & WR_Right ? (1.0f - c / fCols ) : 1.0f );

        const f32 fadeY = 
          m_fadeMode & WR_Top 
          ? ( r / fRows ) 
          : (  m_fadeMode & WR_Bottom ? (1.0f - r / fRows ) : 1.0f );

        const f32 sin = mt::sin_low(t * m_freq + (c + r) * m_diff);
        mesh.SetDisplacement( c, r,
          sin * m_amplitude_x * mt::Min(fadeX, fadeY) ,
          sin * m_amplitude_y * mt::Min(fadeX, fadeY) );
      }
    }
  }

  f32 m_freq;
  f32 m_diff;
  f32 m_amplitude_x;
  f32 m_amplitude_y;
  int m_fadeMode;
  int m_fixMode;
};


class WaterRippler
  : public WaterRipple
{
public:
  WaterRippler(f32 freq, f32 diff, f32 amplitude_x, f32 amplitude_y, int fadeMode, int fixMode)
    : WaterRipple(freq, diff, amplitude_x, amplitude_y, fadeMode, fixMode)
    , m_time(0.0f)
  {  }

  void Update(f32 dt)
  {
    m_time += dt;
  }

  void Apply(GridMesh& mesh)
  {
    WaterRipple::Apply(mesh, m_time);
  }
private:
  f32 m_time;
};

//////////////////////////////////////////////////////////////////////////
// stencil masks
inline Material& WriteMask(Material& m, u8 ref)
{
  m.m_stencil.zpass = Material::StencilState::soReplace;
  m.m_stencil.fun   = Material::ecAlways;
  m.m_stencil.ref   = ref;
  return m;
}

inline Material& ApplyMask(Material& m, u8 ref)
{
  m.m_stencil.fun = Material::ecEqual;
  m.m_stencil.ref = ref;
  return m;
}

NAMESPACE_END(r)

#endif // r_primitives_h 
