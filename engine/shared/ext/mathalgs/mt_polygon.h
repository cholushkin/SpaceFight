#ifndef polygon_h__
#define polygon_h__

#include "core/math/mt_base.h"
#include "core/render/r_render.h"

#include <vector>

NAMESPACE_BEGIN(mt)

i16                       GetSide( mt::v2f const& a, mt::v2f const& b, mt::v2f const& c );
bool                      IsPointInTriangle( mt::v2f const& a, mt::v2f const& b, mt::v2f const& c, mt::v2f const& p );
f32                       GetDistanceToLine( mt::v2f const& p, mt::v2f const& a, mt::v2f const& b );
std::vector<r::Vertex2f>  Triangulate( std::vector<r::Vertex2f> const& vVertices );
bool                      IsConvex( const std::vector<mt::v2f>& shape );
mt::v2f                   IsIntersect2D( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& q0, const mt::v2f& q1 ); // Get intersection of line p0->p1 and line q0->q1.  Not a complete intersection test as it does not test for parallelism.;
mt::v2f                   Nearest2D( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& q0 ); // Get the nearest point on a line p0->p1 to a given point q0.;
int                       LineSide2D( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& q0 ); // Get the side of a line that a given point is on.
mt::v2f                   ComputeMassCentroid( const std::vector<mt::v2f>& shape );
mt::Rectf                 ComputeAABB( const std::vector<r::Vertex2f>& shape );



class Polygon 
{
  std::vector<r::Vertex2f> m_vVertices;
public:
  class iterator 
  {
  private:
    friend class Polygon;

    size_t    m_position;
    Polygon* m_pPoly;

    explicit iterator(Polygon* pPoly, size_t position)
      : m_position(position)
      , m_pPoly(pPoly)
    {
    }
  public:
    explicit iterator()
      : m_position(0)
      , m_pPoly(NULL)
    {
    }

    iterator(iterator const& other)
      : m_position(other.m_position)
      , m_pPoly(other.m_pPoly)
    {      
    }

    iterator& operator ++ () 
    {
      if( m_pPoly )
      {
        size_t const size = m_pPoly->size();
        if( m_position < size )
          m_position = 0 < size ? ( ++m_position % size ) : 0;
      }

      return *this;
    }

    iterator operator ++ (int) 
    {
      iterator tmp(*this);
      ++*this;
      return tmp;
    }

    iterator& operator--() 
    {
      if( m_pPoly )
      {
        size_t const size = m_pPoly->size();
        if( m_position < size )
          m_position = ( 0 == m_position ) ? ( 0 < size ? size - 1 : 0 ) : --m_position;
      }
      return *this;
    }

    iterator operator--(int) 
    {
      iterator tmp(*this);
      --*this;
      return tmp;
    }

    bool operator == ( iterator const& other ) const 
    {
      return m_pPoly == other.m_pPoly && m_position == other.m_position;
    }

    bool operator != ( iterator const& other ) const
    {
      return !(*this == other);
    }

    r::Vertex2f* operator -> () 
    {
      return m_pPoly ? (*m_pPoly)[m_position] : NULL;
    }

    r::Vertex2f& operator * () {
      return *( this-> operator -> ());
    }

  };

  explicit Polygon()
  {
  }

  explicit Polygon( std::vector<r::Vertex2f> const& vVertices) 
    : m_vVertices(vVertices)
  {
  }

  void AddVertex( r::Vertex2f const& v )
  {
    m_vVertices.push_back(v);
  }

  size_t size() const
  {
    return m_vVertices.size();
  }

  r::Vertex2f* operator[] (size_t idx )
  {
    return idx < size() ? &m_vVertices[idx] : NULL;
  }

  r::Vertex2f const* operator[] (size_t idx ) const 
  {
    return idx < size() ? &m_vVertices[idx] : NULL;
  }

  explicit Polygon( iterator _1, iterator _2)
  {
    for(;;) 
    {
      AddVertex( *_1 );
      if( _1 == _2 )
        break;
      ++_1;
    }
  }

  iterator first() 
  {
    return iterator( this, 0 );
  }

  iterator last()
  {
    return iterator( this, size() );
  }

  iterator FindConvexVertex()
  {
    if( first() == last() )
      return last();

    // it`s point, line or triangle: doesn`t  make sense, but return first
    if( size() < 4 )
      return first();

    iterator c = first(), a = c++, b = c++;
    do {
      if( 0 < GetSide(a->coords, b->coords, c->coords) )
        return b;
      ++a, ++b, ++c;
    }
    while(first() != a);
    return last();
  }

  std::vector<r::Vertex2f> Triangulate()
  {
    if( size() < 3 )
      return std::vector<r::Vertex2f>();

    if( size() == 3 )
      return m_vVertices;


    Polygon::iterator convex = FindConvexVertex();
    if( last() == convex ) // not CW poly
      return std::vector<r::Vertex2f>();

    iterator splitStart = convex, splitEnd = last();

    {
      iterator a = convex, b = convex, c = convex;
      --a; ++c;
      iterator v = c;
      ++v;

      float fDistance = -1.0f;
      for( ;v != a; ++v)
      {
        if( !IsPointInTriangle( a->coords, b->coords, c->coords, v->coords) ) 
          continue;

        float d = GetDistanceToLine( v->coords, c->coords, a->coords);
        if( fDistance < d )
        {
          fDistance = d;
          splitEnd  = v;
        }
      }
    }

    if( last() == splitEnd ) 
    {
      --splitStart;
      splitEnd = convex;
      ++splitEnd;
    }

    Polygon _1(splitStart, splitEnd), _2(splitEnd, splitStart);
    std::vector<r::Vertex2f> triangles1 =_1.Triangulate();
    if(triangles1.empty())
      return std::vector<r::Vertex2f>();
    std::vector<r::Vertex2f> triangles2 =_2.Triangulate();
    if(triangles2.empty())
      return std::vector<r::Vertex2f>();

    std::vector<r::Vertex2f> result;
    result.insert( result.end(), triangles1.begin(), triangles1.end() );
    result.insert( result.end(), triangles2.begin(), triangles2.end() );

    return result;
  }


  const std::vector<r::Vertex2f>& getShape() const 
  {
    return m_vVertices;
  }
};


class GPolygon 
{
public:
  GPolygon( const Polygon& poly,  r::Material m, const mt::v2f& texScale = mt::v2f(1,1), const mt::v2f& texOffset = mt::v2f() );
  void Draw(r::Render& r, const mt::v2f& pos, float fScale, float fAngle, bool bHQRot ) const;

  u32                      m_outlineColor; // if alpha is zero then no lines will be rendered
  u32                      m_triangulationLineColor; // if alpha is zero then no lines will be rendered
  u32                      m_aabbLineColor; // if alpha is zero then no lines will be rendered

private:
  mt::Rectf                m_aabb;
  Polygon                  m_poly;
  r::Material              m_mat;
  std::vector<r::Vertex2f> m_triangles;
  u32                      m_frame;
};

NAMESPACE_END(mt)

#endif // polygon_h__