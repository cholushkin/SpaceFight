#include "ext/primitives/r_primitives.h"
#include "ext/algorithm/alg_arrays.h"
#include "core/io/io_base.h"
#include "core/res/res_base.h"
#include "ext/primitives/r_sheet_sprite.h"

using namespace res;
using namespace mt;
using namespace alg;

NAMESPACE_BEGIN(r)

//////////////////////////////////////////////////////////////////////////
// Frame9

void Frame9::Draw(Render& r) const
{
  Draw(r, mt::Rectf(m_tl, m_br), mt::Rectf(0, 1 ,0, 1), mt::Rectf(mt::v2f(0, 0), m_mat.m_pTexture->GetRect()), m_mat, m_opacity);
}

void Frame9::Draw(Render& r, const mt::Rectf& target_rect, const mt::Rectf& uv_rect, const mt::Rectf& source_rect, const r::Material& mat, f32 opacity)
{
  // constants
  static const mt::v2f  g_9slices_uv[9][4] = 
  {
    {v2f( 0,         0),         v2f( ONE_THIRD, 0),         v2f( ONE_THIRD, ONE_THIRD), v2f( 0,         ONE_THIRD)},
    {v2f( ONE_THIRD, 0),         v2f( TWO_THIRD, 0),         v2f( TWO_THIRD, ONE_THIRD), v2f( ONE_THIRD, ONE_THIRD)},
    {v2f( TWO_THIRD, 0),         v2f( 1,         0),         v2f( 1,         ONE_THIRD), v2f( TWO_THIRD, ONE_THIRD)},

    {v2f( 0,         ONE_THIRD), v2f( ONE_THIRD, ONE_THIRD), v2f( ONE_THIRD, TWO_THIRD), v2f( 0,         TWO_THIRD)},
    {v2f( ONE_THIRD, ONE_THIRD), v2f( TWO_THIRD, ONE_THIRD), v2f( TWO_THIRD, TWO_THIRD), v2f( ONE_THIRD, TWO_THIRD)},
    {v2f( TWO_THIRD, ONE_THIRD), v2f( 1,         ONE_THIRD), v2f( 1,         TWO_THIRD), v2f( TWO_THIRD, TWO_THIRD)},

    {v2f( 0,         TWO_THIRD), v2f( ONE_THIRD, TWO_THIRD), v2f( ONE_THIRD, 1),         v2f( 0,         1)},
    {v2f( ONE_THIRD, TWO_THIRD), v2f( TWO_THIRD, TWO_THIRD), v2f( TWO_THIRD, 1),         v2f( ONE_THIRD, 1)},
    {v2f( TWO_THIRD, TWO_THIRD), v2f( 1,         TWO_THIRD), v2f( 1,         1),         v2f( TWO_THIRD, 1)},
  };

  mt::v2f  tl   = target_rect.TL();
  mt::v2f  br   = target_rect.RB();
  mt::v2f  uvtl = uv_rect.TL();
  mt::v2f  uvbr = uv_rect.RB();

  mt::v2f uv_size = uvbr - uvtl;

  mt::v2f slices_uv[9][4];
  for(size_t q = 0; ARRAY_SIZE(g_9slices_uv) != q; ++q)
    for(size_t v = 0; ARRAY_SIZE(g_9slices_uv[q]) != v; ++v)
      slices_uv[q][v] = g_9slices_uv[q][v] * uv_size + uvtl;

  const f32 third_x = source_rect.Width () / 3.0f;
  const f32 third_y = source_rect.Height() / 3.0f;

  const u32 color = make_color(opacity, 255, 255, 255);
  const u32 colors[4] = {color, color, color, color};

  v2f v0[4] = {v2f( tl.x,           tl.y),           v2f( tl.x + third_x, tl.y),           v2f( tl.x + third_x, tl.y + third_y), v2f( tl.x,           tl.y + third_y)};
  v2f v1[4] = {v2f( tl.x + third_x, tl.y),           v2f( br.x - third_x, tl.y),           v2f( br.x - third_x, tl.y + third_y), v2f( tl.x + third_x, tl.y + third_y)};
  v2f v2[4] = {v2f( br.x - third_x, tl.y),           v2f( br.x,           tl.y),           v2f( br.x,           tl.y + third_y), v2f( br.x - third_x, tl.y + third_y)};
  r.PostQuad(v0, slices_uv[0], colors, mat); r.PostQuad(v1, slices_uv[1], colors, mat); r.PostQuad(v2, slices_uv[2], colors, mat);

  v2f v3[4] = {v2f( tl.x,           tl.y + third_y), v2f( tl.x + third_x, tl.y + third_y), v2f( tl.x + third_x, br.y - third_y), v2f( tl.x,           br.y - third_y)};
  v2f v4[4] = {v2f( tl.x + third_x, tl.y + third_y), v2f( br.x - third_x, tl.y + third_y), v2f( br.x - third_x, br.y - third_y), v2f( tl.x + third_x, br.y - third_y)};
  v2f v5[4] = {v2f( br.x - third_x, tl.y + third_y), v2f( br.x,           tl.y + third_y), v2f( br.x,           br.y - third_y), v2f( br.x - third_x, br.y - third_y)};
  r.PostQuad(v3, slices_uv[3], colors, mat); r.PostQuad(v4, slices_uv[4],colors, mat); r.PostQuad(v5, slices_uv[5],colors, mat);

  v2f v6[4] = {v2f( tl.x,           br.y - third_y), v2f( tl.x + third_x, br.y - third_y), v2f( tl.x + third_x, br.y),           v2f( tl.x,           br.y)};
  v2f v7[4] = {v2f( tl.x + third_x, br.y - third_y), v2f( br.x - third_x, br.y - third_y), v2f( br.x - third_x, br.y),           v2f( tl.x + third_x, br.y)};
  v2f v8[4] = {v2f( br.x - third_x, br.y - third_y), v2f( br.x,           br.y - third_y), v2f( br.x,           br.y),           v2f( br.x - third_x, br.y)};
  r.PostQuad(v6, slices_uv[6], colors, mat); r.PostQuad(v7, slices_uv[7],colors, mat); r.PostQuad(v8, slices_uv[8],colors, mat);
}

//////////////////////////////////////////////////////////////////////////
// GridMesh

void GridMesh::Draw(Render& r, const Material& m, const mt::Matrix23f& mtr, const unsigned int color) const
{
  mt::v2f Vs [4];
  mt::v2f UVs[4];
  const u32 acolor[4] = {color, color, color, color};
  for ( u32 j = 1; m_nRows != j ; ++j )
  {
    for ( u32 i = 1; m_nCols != i; ++i )
    {
      u32 idx = j * m_nCols +  i;

      u32 idx1 = idx - m_nCols - 1;
      TransformCoord(Vs [0], m_verts[idx1].v, mtr);
      UVs[0] = m_verts[idx1].uv;

      idx1 = idx - m_nCols;
      TransformCoord(Vs [1], m_verts[idx1].v, mtr);
      UVs[1] = m_verts[idx1].uv;

      idx1 = idx;
      TransformCoord(Vs [2], m_verts[idx1].v, mtr);
      UVs[2] = m_verts[idx1].uv;

      idx1 = idx - 1;
      TransformCoord(Vs [3], m_verts[idx1].v, mtr);
      UVs[3] = m_verts[idx1].uv;

      r.PostQuad(Vs, UVs, acolor, m);
    }
  }
}

void GridMesh::Draw(Render& r, const Material& m, const mt::v2f& pos, const unsigned int color) const
{
  mt::v2f Vs [4];
  mt::v2f UVs[4];
  const u32 acolor[4] = {color, color, color, color};
  for ( u32 j = 1; m_nRows != j ; ++j )
  {
    for ( u32 i = 1; m_nCols != i; ++i )
    {
      u32 idx = j * m_nCols +  i;

      u32 idx1 = idx - m_nCols - 1;
      Vs [ 0 ] = m_verts[ idx1 ].v + pos;
      UVs[ 0 ] = m_verts[ idx1 ].uv;

      idx1 = idx - m_nCols;
      Vs [ 1 ] = m_verts[ idx1 ].v + pos;
      UVs[ 1 ] = m_verts[ idx1 ].uv;

      idx1 = idx;
      Vs [ 2 ] = m_verts[ idx1 ].v + pos;
      UVs[ 2 ] = m_verts[ idx1 ].uv;

      idx1 = idx - 1;
      Vs [ 3 ] = m_verts[ idx1 ].v + pos;
      UVs[ 3 ] = m_verts[ idx1 ].uv;
      r.PostQuad(Vs, UVs, acolor, m);
    }
  }
}

void GridMesh::RebuildGrid()
{
  SAFE_DELETE_ARRAY(m_verts);
  m_verts = new Vertex[m_nRows * m_nCols];

  const mt::v2f size = m_Vs [1] - m_Vs [0];
  const mt::v2f uvsz = m_UVs[1] - m_UVs[0];

  for( u32 row = 0; m_nRows != row; ++row )
  {
    for ( u32 col = 0; m_nCols != col; ++col )
    {
      const u32 ind = row * m_nCols + col;
      const mt::v2f dt(col / (f32)(m_nCols - 1), row / (f32)(m_nRows - 1));

      m_verts[ind].uv = dt * uvsz + m_UVs[0];
      m_verts[ind].v  = dt * size + m_Vs [0];
    }
  }
}

void GridMesh::SetDisplacement( u32 col, u32 row, f32 dx, f32 dy )
{
  EASSERT(row < m_nRows && col < m_nCols);
  if ( row >= m_nRows || col >= m_nCols )
    return;

  const mt::v2f size = m_Vs[1] - m_Vs[0];
  const u32     ind  = row * m_nCols + col;
  const mt::v2f dt(dx + col / (f32)(m_nCols - 1), dy + row / (f32)(m_nRows - 1));

  m_verts[ind].v = dt * size + m_Vs[0];
}

void GridMesh::SetUVDisplacement( u32 col, u32 row, f32 uvdx, f32 uvdy)
{
  EASSERT(row < m_nRows && col < m_nCols);
  if ( row >= m_nRows || col >= m_nCols )
    return;

  const mt::v2f uvsz = m_UVs[1] - m_UVs[0];
  const u32     ind  = row * m_nCols + col;
  const mt::v2f dt(uvdx + col / (f32)(m_nCols - 1), uvdy + row / (f32)(m_nRows - 1));

  m_verts[ind].uv = dt * uvsz + m_UVs[0];
}


//////////////////////////////////////////////////////////////////////////
// TileMap

TileMap::TileMap()
: m_spr(NULL)
, m_data(NULL)
, m_size(0, 0)
{}

TileMap::~TileMap()
{
  SAFE_DELETE_ARRAY(m_data);
  m_spr = NULL;
}

void TileMap::Load(const SheetSprite* spr, u16 w, u16 h, u16* data)
{
  m_spr    = spr;
  m_size.x = w;
  m_size.y = h;
  m_data   = data;
}

void TileMap::Draw(Render& r, const v2f& pos) const
{
  if(!m_spr)
    return;
  const v2f size = m_spr->GetRect(0).RB();
  for(u16 y = 0; m_size.y != y; ++y)
  {
    const f32 fy = size.x * y + pos.y;
    const u32 uy = y * m_size.x;
    for(u16 x = 0; m_size.x != x; ++x)
      if(m_spr->GetNFrames() > m_data[uy + x])
        m_spr->Draw(r, m_data[uy + x], v2f(pos.x + size.x * x, fy), mt::COLOR_WHITE);
  }
}

void TileMap::Draw(Render& r, const v2f& pos, const Rectf& clip) const
{
  if(!m_spr)
    return;
  const v2f size = m_spr->GetRect(0).RB();
  for(u16 y = 0; m_size.y != y; ++y)
  {
    const f32 fy = size.x * y + pos.y;
    if(clip.bottom < fy)
      continue;
    if(clip.top > fy + size.y)
      continue;
    const u32 uy = y * m_size.x;
    for(u16 x = 0; m_size.x != x; ++x)
    {
      const f32 fx = pos.x + size.x * x;
      if(clip.right < fx)
        continue;
      if(clip.left > fx + size.y)
        continue;
      m_spr->Draw(r, m_data[uy + x], v2f(fx, fy), mt::COLOR_WHITE);
    }
  }
}

NAMESPACE_END(r)