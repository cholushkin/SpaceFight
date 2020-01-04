#include "core/render/r_render.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
using namespace mt;

NAMESPACE_BEGIN(r)


void CalculateBezier( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& p2, const mt::v2f& p3, u32 steps, /*out*/ v2f* ret );


DrawHelper::DrawHelper( r::Render& r )
:  m_render(r)
,  m_rotator(0,true)
,  m_scale(1.0f)
,  m_translate(v2f()) {
}

DrawHelper& DrawHelper::DrawPoly( const Polygon& poly ) {
  EASSERT(poly.size()>2);
  //for(Polygon::iterator i = poly.first(); i!=poly.last(); ++i)

  const r::Vertex2f* vx[2] = {NULL,poly[poly.size()-1]};
  r::Material m;


  Rectf aabb = ComputeAABB( poly.getShape() );
  const v2f origin(aabb.left + aabb.Width()*0.5f, aabb.top + aabb.Height()*0.5f);

  for(u32 i = 0; i < poly.size(); ++ i) {
    vx[0] = vx[1];
    vx[1] = poly[i];
    v2f coords[2] = {vx[0]->coords,vx[1]->coords};
    u32 colors[2] = {vx[0]->color,vx[1]->color};
    TransformPoint(coords[0],origin);
    TransformPoint(coords[1],origin);
    m_render.PostLine(coords,colors,m);
  }

  return *this;
}

DrawHelper& DrawHelper::DrawLine( const v2f& p1, const v2f& p2, const u32& color ) {
  r::Material m;
  v2f coords[2] = { p1, p2 };
  u32 colors[2] = { color, color };
  v2f origin = (p1 + p2) / 2;
  TransformPoint(coords[0],origin);
  TransformPoint(coords[1],origin);
  m_render.PostLine(coords,colors,m);

  return *this;
}

DrawHelper& DrawHelper::FillRect( const Rectf& rect, const u32& color ) {
  r::Material m;
  const v2f def_uv[4] = {v2f(0, 0), v2f(1, 0), v2f(1, 1), v2f( 0, 1)};
  const u32 colors[4] = {color,color,color,color};
  v2f origin(rect.left+rect.Width()/2, rect.top+rect.Height()/2);
  v2f points[4] = {v2f(rect.left,rect.top),v2f(rect.right,rect.top),v2f(rect.right,rect.bottom),v2f(rect.left,rect.bottom)};
  
  TransformPoint(points[0],origin);
  TransformPoint(points[1],origin);
  TransformPoint(points[2],origin);
  TransformPoint(points[3],origin);

  m_render.PostQuad(points, def_uv, colors, m);

  return *this;
}

DrawHelper& DrawHelper::FillRect( const Rectf& rect, const u32 colors[4]) {
  r::Material m;
  const v2f def_uv[4] = {v2f(0, 0), v2f(1, 0), v2f(1, 1), v2f( 0, 1)};
  v2f origin(rect.left+rect.Width()/2, rect.top+rect.Height()/2);
  v2f points[4] = {v2f(rect.left,rect.top),v2f(rect.right,rect.top),v2f(rect.right,rect.bottom),v2f(rect.left,rect.bottom)};

  TransformPoint(points[0],origin);
  TransformPoint(points[1],origin);
  TransformPoint(points[2],origin);
  TransformPoint(points[3],origin);

  m_render.PostQuad(points, def_uv, colors, m);

  return *this;
}


DrawHelper& DrawHelper::DrawRect( const Rectf& rect, const u32& color ) {
  r::Material m;
  v2f origin(rect.left+rect.Width()/2, rect.top+rect.Height()/2);
  v2f coordsL[2] = { v2f(rect.left,rect.top), v2f(rect.left,rect.bottom) };
  v2f coordsR[2] = { v2f(rect.right,rect.top), v2f(rect.right,rect.bottom)};
  v2f coordsT[2] = { v2f(rect.left,rect.top), v2f(rect.right,rect.top) };
  v2f coordsB[2] = { v2f(rect.left,rect.bottom), v2f(rect.right,rect.bottom) };
  u32 colors[2] = { color, color };

  for(int i = 0; i < 2; ++i) {
    TransformPoint(coordsL[i], origin );
    TransformPoint(coordsR[i], origin );
    TransformPoint(coordsT[i], origin );
    TransformPoint(coordsB[i], origin );
  }

  m_render.PostLine(coordsL,colors,m);
  m_render.PostLine(coordsT,colors,m);
  m_render.PostLine(coordsR,colors,m);
  m_render.PostLine(coordsB,colors,m);

  return *this;
}


DrawHelper& DrawHelper::FillCircle( const v2f& center, f32 radius, u16 segmentCnt, const u32& color ) {
  EASSERT(segmentCnt >= 3);

  v2f pos;
  const u32 colors[] = { color, color, color };
  v2f coords[] = {v2f(),center + v2f(radius,0) + m_translate, center+m_translate};
  const v2f def_uv[3] = {v2f(0, 0), v2f(1, 0), v2f(1, 1)};
  r::Material m;

  const f32 angleStep = (f32)(M_2PI / segmentCnt);

  for(u32 i = 1; i <= segmentCnt; ++i) {
    Rotator rot( angleStep * i, true );
    pos = center + rot.Rotate( v2f(radius,0) ) + m_translate;
    coords[0] = coords[1];
    coords[1] = pos;
    m_render.PostTri(coords,def_uv,colors,m);
  }

  return *this;
}

DrawHelper& DrawHelper::DrawCircle( const v2f& center, f32 radius, u16 segmentCnt, const u32& color  )
{
  EASSERT(segmentCnt >= 3);

  v2f pos;
  const u32 colors[2] = { color, color };
  v2f coords[2] = {v2f(),center + v2f(radius,0) + m_translate};
  r::Material m;

  const f32 angleStep = (f32)(M_2PI / segmentCnt);

  for(u32 i = 1; i <= segmentCnt; ++i) {
    Rotator rot( angleStep * i, true );
    pos = center + rot.Rotate( v2f(radius,0) ) + m_translate;
    coords[0] = coords[1];
    coords[1] = pos;
    m_render.PostLine(coords,colors,m);
  }

  return *this;
}

DrawHelper& DrawHelper::DrawStar( const v2f& center, f32 radius, u16 segmentCnt, const u32& color ) {
  EASSERT(segmentCnt >= 3);

  v2f pos;
  const u32 colors[2] = { color, color };
  v2f coords[2] = {v2f(),center + m_translate};
  r::Material m;

  const f32 angleStep = (f32)(M_2PI / segmentCnt);

  for(u32 i = 0; i < segmentCnt; ++i) {
    Rotator rot(angleStep * i,false);
    pos = center + rot.Rotate( v2f(radius,0) ) + m_translate;
    coords[0] = pos;
    m_render.PostLine(coords,colors,m);
  }

  return *this;
}

DrawHelper& DrawHelper::SetTransform( const v2f& translate, f32 scale, f32 angle, bool isHighPrecision) {
  m_rotator.setAngle(angle,isHighPrecision);
  m_translate = translate;
  m_scale = scale;

  return *this;
}

DrawHelper& DrawHelper::ClearTansform() {
  m_rotator.setAngle(0,false);
  m_translate = v2f();

  return *this;
}

void DrawHelper::TransformPoint( v2f& v, const v2f& rotOrigin ) {
  v -= rotOrigin; // move to origin
  v =  m_rotator.Rotate(v); // rotate
  v += rotOrigin + m_translate; // move back & apply translation
}

DrawHelper& DrawHelper::DrawBezier (
  const mt::v2f& p1,
  const mt::v2f& p2,
  const mt::v2f& p3,
  const mt::v2f& p4,
  const u32& color, 
  u32 steps )
{
  static const u32 MAX_STEPS = 64;
  static v2f arr[MAX_STEPS];
  steps = mt::Clamp<u32>(steps,1,MAX_STEPS);
  CalculateBezier(p1,p2,p3,p4,steps,arr);

  r::Material m;

//   todo: render this in aux mode (axis, origins)
//   DrawCircle(p1,8,8,mt::COLOR_DARKGREEN);
//   DrawCircle(p2,8,8,mt::COLOR_LIGHTGREEN);
//   DrawCircle(p3,8,8,mt::COLOR_LIGHTGREEN);
//   DrawCircle(p4,8,8,mt::COLOR_DARKGREEN);

  u32 colors[2] = {color,color};
  for(u32 i = 0; i < steps-1; ++ i) 
  {
    v2f coords[2] = {arr[i],arr[i+1]};
    
    m_render.PostLine(coords,colors,m);
  }
  return *this;
}

void DrawHelper::CalculateBezier( const mt::v2f& p0, const mt::v2f& p1, const mt::v2f& p2, const mt::v2f& p3, u32 steps, /*out*/ v2f* ret )
{
  if (steps == 0) 
    return;

  v2f f, fd, fdd, fddd, fdd_per_2, fddd_per_2, fddd_per_6;
  f32 t = 1.0f / f32(steps-1);
  f32 t2 = t * t;

  f = p0;
  fd =  t * (p1 - p0)*3.0f;
  fdd_per_2 = t2 * (p0 - 2.0f * p1 + p2) * 3.0f;
  fddd_per_2 = t2 * t * (3.0f * (p1 - p2) + p3 - p0) * 3.0f;

  fddd = fddd_per_2 + fddd_per_2;
  fdd = fdd_per_2 + fdd_per_2;
  fddd_per_6 = (1.0f / 3) * fddd_per_2;

  if (!ret) 
    return;

  for (u32 loop=0; loop < steps; loop++) 
  {
    ret[loop] = f;

    f = f + fd + fdd_per_2 + fddd_per_6;
    fd = fd + fdd + fddd_per_2;
    fdd = fdd + fddd;
    fdd_per_2 = fdd_per_2 + fddd_per_2;
  }
}


NAMESPACE_END(r)