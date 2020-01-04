#ifndef draw_helper_h__
#define draw_helper_h__

#include "ext/mathalgs/mt_polygon.h"

NAMESPACE_BEGIN(r)

class Render;

class DrawHelper 
{
  DENY_COPY(DrawHelper);
public:
  DrawHelper( r::Render& r );
  DrawHelper& DrawPoly( const mt::Polygon& poly );
  // todo:
  // fillpoly
  // drawTriangle
  // fillTriangle
  
  DrawHelper& DrawLine( const mt::v2f& p1, const mt::v2f& p2, const u32& color );
  DrawHelper& DrawBezier( 
    const mt::v2f& p1,
    const mt::v2f& p2,
    const mt::v2f& p3,
    const mt::v2f& p4,
    const u32& color, 
    u32 steps = 30 
  );

  DrawHelper& FillRect( const mt::Rectf& rect, const u32& color );
  DrawHelper& FillRect( const mt::Rectf& rect, const u32 color[4]);
  DrawHelper& DrawRect( const mt::Rectf& rect, const u32& color );

  DrawHelper& FillCircle( const mt::v2f& center, f32 radius, u16 segmentCnt, const u32& color );
  DrawHelper& DrawCircle( const mt::v2f& center, f32 radius, u16 segmentCnt, const u32& color );

  DrawHelper& DrawStar( const mt::v2f& center, f32 radius, u16 segmentCnt, const u32& color );
  
  DrawHelper& SetTransform( const mt::v2f& translate, f32 scale = 1.0f, f32 angle = 0.0f, bool isHighPrecision = false );
  DrawHelper& ClearTansform();


  static void CalculateBezier( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& p2, const mt::v2f& p3, u32 steps, /*out*/ mt::v2f* ret );
private:
  void TransformPoint( mt::v2f& v, const mt::v2f& rotOrigin = mt::v2f() );
  mt::Rotator     m_rotator;
  mt::v2f         m_translate;
  f32             m_scale;
  r::Render&      m_render;
};


NAMESPACE_END(r)
#endif // draw_helper_h__