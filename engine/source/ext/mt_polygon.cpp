#include "ext/mathalgs/mt_polygon.h"
#include "config.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"

NAMESPACE_BEGIN(mt)

static const f32 gGeometry_Epsilon = 0.00001f;

i16 GetSide( v2f const& a, v2f const& b, v2f const& c )
{
  v2f const _1 = c - a;
  v2f const _2 = c - b;

  return (_1.x * _2.y - _2.x * _1.y) < 0.0f ? -1 : 1;
}

bool IsPointInTriangle( v2f const& a, v2f const& b, v2f const& c, v2f const& p )
{
  return 0 < GetSide( a, b, p ) && 0 < GetSide( b, c, p ) && 0 < GetSide( c, a, p );
}

f32 GetDistanceToLine( v2f const& p, v2f const& a, v2f const& b )
{
  const float distA = ( a - p ).length();
  const float distB = ( b - p ).length();

  if( b.x == a.x && b.y == a.y )
    return distA;

  v2f const dp1 = ( p - a ) * ( b - a );
  v2f const dp2 = ( p - b ) * ( b - a );

  if( (dp1.x + dp1.y) <= 0.0f || 0.0f <= (dp2.x + dp2.y) )
    return distA < distB ? distA : distB;

  const float divider  = (float) sqrt( pow(b.x - a.x, 2) + pow(b.y - a.y, 2) );
  float distance       =
    (a.y - b.y) * p.x + (b.x - a.x) * p.y + ( a.x * b.y - a.y * b.x );

  distance /= divider;

  return fabs(distance);
}

std::vector<r::Vertex2f> Triangulate( std::vector<r::Vertex2f> const& vVertices )
{
  return Polygon(vVertices).Triangulate();
}

bool IsConvex( const std::vector<v2f>& shape )
{
  if(shape.size() <= 3)
    return true;

  const size_t size = shape.size();
  const int side = LineSide2D(shape[0], shape[1], shape[2]);

  for(size_t i = 0; size != i; ++i)
  {
    const size_t p0 = i       % size;
    const size_t p1 = (i + 1) % size;
    for (size_t j = 0; shape.size() != j; ++j)
    {
      if(j == p0 || j == p1)
        continue;

      if(LineSide2D(shape[p0], shape[p1], shape[j]) != side)
        return false;
    }
  }
  return true;
}

v2f IsIntersect2D( const v2f& p0, const v2f& p1, const v2f& q0, const v2f& q1 ) /* Get intersection of line p0->p1 and line q0->q1. Not a complete intersection test as it does not test for parallelism. */
{
  const v2f u( p1.x - p0.x, p1.y - p0.y );
  const v2f v( q1.x - q0.x, q1.y - q0.y );
  const v2f w( p0.x - q0.x, p0.y - q0.y );
  const f32 s = ( v.y * w.x - v.x * w.y ) / ( v.x * u.y - v.y * u.x );
  return v2f( p0.x + u.x * s, p0.y + u.y * s );
}

v2f Nearest2D( const v2f& p0, const v2f& p1, const v2f& q0 ) /* Get the nearest point on a line p0->p1 to a given point q0. */
{
  const v2f u( p1.x - p0.x, p1.y - p0.y );
  const v2f n( -u.y, u.x );
  const v2f q1( q0.x + n.x, q0.y + n.y );
  return IsIntersect2D( p0, p1, q0, q1 );
}

int LineSide2D( const v2f& p0, const v2f& p1, const v2f& q0 )
{
  const v2f nearest = Nearest2D( p0, p1, q0 );
  const v2f u( p1.x - p0.x, p1.y - p0.y );
  const v2f n( -u.y, u.x );
  const v2f v( q0.x - nearest.x, q0.y - nearest.y );

  float dot = n.x * v.x + n.y * v.y;
  if ( dot < gGeometry_Epsilon )
    return -1;
  if ( dot > gGeometry_Epsilon )
    return 1;
  return 0;
}

v2f ComputeMassCentroid( const std::vector<v2f>& shape )
{
  EASSERT(shape.size() >= 2);
  v2f c;
  f32 area = 0.0f;
  if (shape.size() == 2)
    return 0.5f * (shape[0] + shape[1]);

  v2f pRef(0.0f, 0.0f);
  const f32 inv3 = 1.0f / 3.0f;

  for (u32 i = 0; i < shape.size(); ++i)
  {
    // Triangle vertices.
    v2f p1 = pRef;
    v2f p2 = shape[i];
    v2f p3 = i + 1 < shape.size() ? shape[i+1] : shape[0];

    v2f e1 = p2 - p1;
    v2f e2 = p3 - p1;

    f32 D = e1.crossProduct(e2);

    f32 triangleArea = 0.5f * D;
    area += triangleArea;

    // Area weighted centroid
    c += triangleArea * inv3 * (p1 + p2 + p3);
  }

  // Centroid
  EASSERT(area > gGeometry_Epsilon);
  c *= 1.0f / area;
  return c;
}

Rectf ComputeAABB( const std::vector<r::Vertex2f>& shape )
{
  EASSERT(shape.size() >= 2);
  v2f mins = shape[0].coords;
  v2f maxs = shape[0].coords;
  
  for (u32 i = 0; i < shape.size(); ++i)
  {
    if( shape[i].coords.x < mins.x ) 
      mins.x = shape[i].coords.x;
    if( shape[i].coords.y < mins.y ) 
      mins.y = shape[i].coords.y;
    if( shape[i].coords.x > maxs.x )
      maxs.x = shape[i].coords.x;
    if( shape[i].coords.y > maxs.y )
      maxs.y = shape[i].coords.y;
  }

  return Rectf(mins.x,maxs.x,mins.y,maxs.y);
}




// ----- GPolygon
GPolygon::GPolygon( const Polygon& poly, r::Material m, const v2f& texScale, const v2f& texOffset )
:  m_poly(poly)
,  m_mat(m)
,  m_outlineColor(0)
,  m_triangulationLineColor(0)
,  m_aabbLineColor(0)
{
  m_aabb      = ComputeAABB(m_poly.getShape());
  m_triangles = m_poly.Triangulate();

  // uv mapping
  for( u32 i = 0; i < m_triangles.size(); ++i ) {
    m_triangles[i].uv.x = m_triangles[i].coords.x - m_aabb.left;
    m_triangles[i].uv.y = m_triangles[i].coords.y - m_aabb.top;
    m_triangles[i].uv.x = m_triangles[i].uv.x/(m_aabb.Width()*texScale.x) + texOffset.x;
    m_triangles[i].uv.y = m_triangles[i].uv.y/(m_aabb.Height()*texScale.y) + texOffset.y;    
  }
}


void GPolygon::Draw(r::Render& r, const v2f& pos, float fScale, float fAngle, bool bHQRot ) const
{
  const Rotator rot(fAngle, bHQRot);

  for(u32 i=0; i<m_triangles.size(); i+=3 ) 
  {
    const v2f coords[3] = {
      rot.Rotate( m_triangles[i].coords * fScale ) + pos,
      rot.Rotate( m_triangles[i+1].coords * fScale ) + pos,
      rot.Rotate( m_triangles[i+2].coords * fScale ) + pos,
    };

    const v2f uv[3] = {
      m_triangles[i].uv,
      m_triangles[i+1].uv,
      m_triangles[i+2].uv,
    };

    const u32 colors[3] = {
      m_triangles[i].color, 
      m_triangles[i+1].color, 
      m_triangles[i+2].color
    };

    r.PostTri(coords, uv, colors, m_mat);
  }

  // draw triangulualtion
  if(GETA(m_triangulationLineColor)!=0)
  {    
    r::DrawHelper dr(r);
    for(u32 i=0; i<m_triangles.size(); i+=3 ) 
    {
      const v2f coords[3] = {
        rot.Rotate( m_triangles[i].coords * fScale ) + pos,
        rot.Rotate( m_triangles[i+1].coords * fScale ) + pos,
        rot.Rotate( m_triangles[i+2].coords * fScale ) + pos,
      };

      dr.DrawLine(coords[0],    coords[1],  m_triangulationLineColor);
      dr.DrawLine(coords[1],    coords[2],  m_triangulationLineColor);
      dr.DrawLine(coords[2],    coords[0],  m_triangulationLineColor);
    }
  }
  // draw outline
  if(GETA(m_outlineColor)!=0)
  {
    r::DrawHelper dr(r);
    for(u32 i=0; i<m_poly.size(); ++i ) 
    {
      v2f p1 = rot.Rotate(m_poly[i]->coords * fScale) + pos;
      v2f p2 = rot.Rotate(m_poly[i==m_poly.size()-1?0:i+1]->coords * fScale) + pos;
      dr.DrawLine( p1, p2, m_outlineColor );
    }
  }
  // todo: draw aabb 
  if(GETA(m_aabbLineColor)!=0)
  {
    r::DrawHelper dr(r);
    const v2f coords[4] = {
      rot.Rotate( v2f(m_aabb.left,m_aabb.top)*fScale ) + pos,
      rot.Rotate( v2f(m_aabb.right,m_aabb.top)*fScale ) + pos,
      rot.Rotate( v2f(m_aabb.right,m_aabb.bottom)*fScale ) + pos,
      rot.Rotate( v2f(m_aabb.left,m_aabb.bottom)*fScale ) + pos
    };

    dr.DrawLine(coords[0],    coords[1],  m_aabbLineColor);
    dr.DrawLine(coords[1],    coords[2],  m_aabbLineColor);
    dr.DrawLine(coords[2],    coords[3],  m_aabbLineColor);
    dr.DrawLine(coords[3],    coords[0],  m_aabbLineColor);
  }
}

NAMESPACE_END(mt)