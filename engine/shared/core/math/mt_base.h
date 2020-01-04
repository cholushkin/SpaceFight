
#ifndef _mt_base_h 
#define _mt_base_h 


#include "core/common/com_misc.h"
#include "core/common/com_types.h"
#include "core/math/mt_vmath.h"
#include <limits>
#include <algorithm>

NAMESPACE_BEGIN(mt)

// bit operations
#define IS_ODD( x )   ( ( (x) & 0x1 ) != 0 )
#define IS_EVEN( x )  ( ( (x) & 0x1 ) == 0 )
// todo: is power of two, next power of two, prev power of two, get number of bits


const f32 MF_PI((f32)M_PI);


typedef Vector2<i8>  v2i8;
typedef Vector2<u8>  v2u8;
typedef Vector3<i8>  v3i8;
typedef Vector3<u8>  v3u8;

typedef Vector2<u16> v2u16;
typedef Vector2<u32> v2u32;
typedef Vector2<i16> v2i16;
typedef Vector3<i16> v3i16;

typedef Vector2<f32> v2f;
typedef Vector3<f32> v3f;
typedef Vector4<f32> v4f;

typedef Matrix3 <f32> Matrix3f;
typedef Matrix23<f32> Matrix23f;
typedef Matrix4 <f32> Matrix4f;

typedef Quaternion<f32> Quaternionf;

template<typename T> 
T Clamp(T value, T minValue, T maxValue)
{
  return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}

template<typename T> 
inline T Clamp360(T u)
{
  while(u < 0) // replace me with fmod
    u += 360;
  while(u > 360)
    u -= 360;
  return u;
}

template<>
inline f32 Clamp360<f32>(f32 u)
{
  while(u < 0) // replace me with fmod
    u += 360;
  if(u > 360)
    u = fmodf(u, 360);
  return u;
}

template<typename T> 
T Min(T l, T r )
{
  return l < r ? l : r;
}

template<typename T> 
T Max(T l, T r )
{
  return l < r ? r : l;
}

//////////////////////////////////////////////////////////////////////////
// fast math

inline u32 sqdist(i16 x1, i16 y1, i16 x2, i16 y2)
{
	int xx((x1 - x2));
	int yy((y1 - y2));
	return xx*xx + yy*yy;
}

inline i8 abs8(i8 i)
{
	return i > 0 ? i : -i;
}

inline i32 abs32(i32 i)
{
	return i > 0 ? i : -i;
}

// fast sqrt and reciprocal sqrt
// works on float since f32 can be fixed
inline float fastsqrt(float val)  
{
  union
  {
    int tmp;
    float val;
  } u;
  u.val = val;
  u.tmp -= 1<<23; /* Remove last bit so 1.0 gives 1.0 */
  /* tmp is now an approximation to logbase2(val) */
  u.tmp >>= 1; /* divide by 2 */
  u.tmp += 1<<29; /* add 64 to exponent: (e+127)/2 =(e/2)+63, */
  /* that represents (e/2)-64 but we want e/2 */
  return u.val;
}

inline float invSqrt(float x)
{
  float xhalf = 0.5f*x;
  union
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = 0x5f3759df - (u.i >> 1);
  x = u.x * (1.5f - xhalf * u.x * u.x);
  return x;
}

inline float ffast_cos(const float x)
{
  // assert:  0 <= fT <= PI/2
  // maximum absolute error = 1.1880e-03
  // speedup = 2.14
  float x_sqr = x*x;
  float res = float(3.705e-02);
  res *= x_sqr;
  res -= float(4.967e-01);
  res *= x_sqr;
  res += 1.0f;
  return res;
}


inline float fast_cos(const float x)
{
  // assert:  0 <= fT <= PI/2
  // maximum absolute error = 2.3082e-09
  // speedup = 1.47
  float x_sqr = x*x;
  float res = float(-2.605e-07);
  res *= x_sqr;
  res += float(2.47609e-05);
  res *= x_sqr;
  res -= float(1.3888397e-03);
  res *= x_sqr;
  res += float(4.16666418e-02);
  res *= x_sqr;
  res -= float(4.999999963e-01);
  res *= x_sqr;
  res += 1.0f;
  return res;
}


//Low precision sine (~14x faster)
inline f32 sin_low(f32 x)
{
  //always wrap input angle to -PI..PI
  if (x < -6.28318531f)
    x = -fmodf(-x, 6.28318531f);
  else if (x > 6.28318531f)
    x = fmodf(x, 6.28318531f);

  if (x < -3.14159265f)
    x += 6.28318531f;
  if (x > 3.14159265f)
    x -= 6.28318531f;

  //compute sine
  return (x < 0.0f)
    ? 1.27323954f * x + 0.405284735f * x * x
    : 1.27323954f * x - 0.405284735f * x * x;
}


//Low precision sine/cosine (~14x faster)
inline void sin_cos_low(f32 x, f32& sin, f32& cos)
{
  //always wrap input angle to -PI..PI
  if (x < -6.28318531f)
    x = -fmodf(-x, 6.28318531f);
  else if (x > 6.28318531f)
    x = fmodf(x, 6.28318531f);

  if (x < -3.14159265f)
    x += 6.28318531f;
  if (x > 3.14159265f)
    x -= 6.28318531f;

  //compute sine
  if (x < 0.0f)
    sin = 1.27323954f * x + 0.405284735f * x * x;
  else
    sin = 1.27323954f * x - 0.405284735f * x * x;

  //compute cosine: sin(x + PI/2) = cos(x)
  x += 1.57079632f;
  if (x > 3.14159265f)
    x -= 6.28318531f;

  if (x < 0.0f)
    cos = 1.27323954f * x + 0.405284735f * x * x;
  else
    cos = 1.27323954f * x - 0.405284735f * x * x;
}


//High precision sine/cosine (~8x faster)
inline void sin_cos_high(f32 x, f32& sin, f32& cos)
{
  //always wrap input angle to -PI..PI
  if (x < -6.28318531f)
    x = -fmodf(-x, 6.28318531f);
  else if (x > 6.28318531f)
    x = fmodf(x, 6.28318531f);

  if (x < -3.14159265f)
    x += 6.28318531f;
  if (x > 3.14159265f)
    x -= 6.28318531f;

  //compute sine
  if (x < 0.0f)
  {
    sin = 1.27323954f * x + 0.405284735f * x * x;

    if (sin < 0.0f)
      sin = 0.225f * (sin *-sin - sin) + sin;
    else
      sin = 0.225f * (sin * sin - sin) + sin;
  }
  else
  {
    sin = 1.27323954f * x - 0.405284735f * x * x;

    if (sin < 0.0f)
      sin = 0.225f * (sin *-sin - sin) + sin;
    else
      sin = 0.225f * (sin * sin - sin) + sin;
  }

  //compute cosine: sin(x + PI/2) = cos(x)
  x += 1.57079632f;
  if (x >  3.14159265f)
    x -= 6.28318531f;

  if (x < 0.0f)
  {
    cos = 1.27323954f * x + 0.405284735f * x * x;

    if (cos < 0.0f)
      cos = 0.225f * (cos *-cos - cos) + cos;
    else
      cos = 0.225f * (cos * cos - cos) + cos;
  }
  else
  {
    cos = 1.27323954f * x - 0.405284735f * x * x;

    if (cos < 0.0f)
      cos = 0.225f * (cos *-cos - cos) + cos;
    else
      cos = 0.225f * (cos * cos - cos) + cos;
  }
}


inline void normalize_fast(v2f& v)
{
  const f32 s = invSqrt(v.lengthSq());
  v.x *= s;
  v.y *= s;
}

inline unsigned char flt_to_byte(float a)
{
  float x = a + 256.0f;
  return (unsigned char)(((*(int*)&x)&0x7fffff)>>15);
}

inline unsigned int flt_to_byte_as_uint(float a)
{
  float x = a + 256.0f;
  return ((*(unsigned int*)&x)&0x7fffff)>>15;
}


// x = (1 + mant) * 2 ^ (exp - 127)

inline int float_to_int(float x)
{
  unsigned int e = (0x7F + 31) - ((* (unsigned int*) &x & 0x7F800000) >> 23);
  unsigned int m = 0x80000000 | (* (unsigned int*) &x << 8);
  return int((m >> e) & -(e < 32));
}

struct IEEE754FLOAT
{
  union
  {
    float x;
    struct
    {
      unsigned int mant : 23; /* Mantissa without leading one */ 
      unsigned int exp  : 8;  /* Exponential part */
      unsigned int sign : 1;  /* Indicator of the negative number */
    } ieee754;
  };
};

#define LOG2E 1.44269504088896340736f

inline float fastLog2( float x )
{
  IEEE754FLOAT ax;
  int exp;

  ax.x = x;
  exp = ax.ieee754.exp - 127;
  ax.ieee754.sign = 0;
  ax.ieee754.exp = 127;

  return (ax.x - 1.0f) * LOG2E + exp;
}

inline u32 nextpow2(u32 val)
{
  --val;
  val = (val >>  1) | val;
  val = (val >>  2) | val;
  val = (val >>  4) | val;
  val = (val >>  8) | val;
  val = (val >> 16) | val;
  ++val;
  return val;
}

inline double round(double r) {
  return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

inline float roundf(float r) {
  return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}

inline u16 sqrt_newton(u32 l)
{
  u32 div;
  if(l & 0xFFFF0000L)
    if (l & 0xFF000000L)
      div = 0x3FFF;
    else
      div = 0x3FF;
  else if(l & 0x0FF00L)
    div = 0x3F;
  else
    div = (l > 4) ? 0x7 : l;
  u32 rslt = l;
  for (;;)
  {
    u32 temp = l / div + div;
    div =  temp >> 1;
    div += temp  & 1;
    if(rslt > div)
      rslt = div;
    else
    {
      if(l / rslt == rslt - 1 && l % rslt == 0)
        --rslt;
      return (u16)rslt;
    }
  }
}

//////////////////////////////////////////////////////////////////////////

class Rotator
{
  f32 m_sin;
  f32 m_cos;
public:
  Rotator(f32 angle_rad, bool bHigh = false)
  {
    setAngle(angle_rad,bHigh);
  }
  void setAngle(f32 angle_rad, bool bHigh)
  {
    if(bHigh)
      sin_cos_high(angle_rad, m_sin, m_cos);
    else
      sin_cos_low(angle_rad, m_sin, m_cos);
  }
  const v2f Rotate(const v2f& v) const
  {
    return v2f(
      v.x * m_cos - v.y * m_sin,
      v.x * m_sin + v.y * m_cos);
  }
};

//////////////////////////////////////////////////////////////////////////

inline v2f& TransformCoord(v2f& o, const v2f& i, const Matrix3f& m)
{
  f32 x, y;
  x = m.m[0][0]*i.x + m.m[1][0]*i.y + m.m[2][0];
  y = m.m[0][1]*i.x + m.m[1][1]*i.y + m.m[2][1];
  o.x = x;
  o.y = y;
  return o;
}

inline v2f TransformCoord(const v2f& i,const Matrix3f& m)
{
  return v2f(m.m[0][0]*i.x + m.m[1][0]*i.y + m.m[2][0],
             m.m[0][1]*i.x + m.m[1][1]*i.y + m.m[2][1]);
}

inline v2f& TransformCoord(v2f& o, const v2f& i, const Matrix23f& m)
{
  m.transform(i, o);
  return o;
}

inline v2f TransformCoord(const v2f& i,const Matrix23f& m)
{
  v2f o;
  m.transform(i, o);
  return o;
}

//////////////////////////////////////////////////////////////////////////
// Rect $$$ Ink: Not tested at all

template<class T>
struct RectT
{
  RectT();
  RectT(T left, T right, T top, T bottom);
  RectT(const Vector2<T>& tl, const Vector2<T>& rb);

  template<class D>
  explicit RectT(const RectT<D>& d)
    : left((T)d.left)
    , top ((T)d.top)
    , right ((T)d.right)
    , bottom((T)d.bottom)
  {
  }

  bool isIn(const Vector2<T>&) const;
  bool isIn(T x, T y) const;

  RectT<T>& Validate();
  bool isValid() const;
  bool isIntertsect(const RectT<T>&) const;

  Vector2<T>&       TL()       { return *(reinterpret_cast< Vector2<T>       * >(&left )); }
  Vector2<T>&       RB()       { return *(reinterpret_cast< Vector2<T>       * >(&right)); }
  Vector2<T> const& TL() const { return *(reinterpret_cast< Vector2<T> const * >(&left )); }
  Vector2<T> const& RB() const { return *(reinterpret_cast< Vector2<T> const * >(&right)); }

  RectT<T>& Cover(const Vector2<T>& pt);
  RectT<T>& Cover(const RectT<T>&);

  T Width () const { return right  - left; }
  T Height() const { return bottom - top; }
  T Area  () const { return Width() * Height(); }
  Vector2<T> Size() const { return Vector2<T>(Width(), Height()); }

  RectT<T>& Offset(const Vector2<T>& offset) {
    left   += offset.x;
    right  += offset.x;
    top    += offset.y;
    bottom += offset.y;
    return *this;
  }

  RectT<T>& Offset(T dx, T dy) {
    left   += dx;
    right  += dx;
    top    += dy;
    bottom += dy;
    return *this;
  }

  RectT<T>& Expand(T offset) {
    left   -= offset;
    right  += offset;
    top    -= offset;
    bottom += offset;
    return *this;
  }

  RectT<T>& Expand(T x_offset, T y_offset) {
    left   -= x_offset;
    right  += x_offset;
    top    -= y_offset;
    bottom += y_offset;
    return *this;
  }

  RectT<T>& Scale(f32 s) {
    left   *= (T)s;
    right  *= (T)s;
    top    *= (T)s;
    bottom *= (T)s;
    return *this;
  }

  RectT<T>& Scale(const Vector2<T>& s) {
    left   *= (T)s.x;
    right  *= (T)s.x;
    top    *= (T)s.y;
    bottom *= (T)s.y;
    return *this;
  }

  // CW: TL, TR, BR, BL
  void Vertices(Vector2<T> p[4]) const {
    p[0] = Vector2<T>(left,  top);
    p[1] = Vector2<T>(right, top);
    p[2] = Vector2<T>(right, bottom);
    p[3] = Vector2<T>(left,  bottom);
  }

  T left;
  T top;
  T right;
  T bottom;
};

template<class T>
inline RectT<T>::RectT()
: left(0)
, right(0)
, top(0)
, bottom(0)
{}

template<class T>
inline RectT<T>::RectT(T left, T right, T top, T bottom)
: left(left)
, right(right)
, top(top)
, bottom(bottom)
{}

template<class T>
inline RectT<T>::RectT(const Vector2<T>& tl, const Vector2<T>& rb)
: left(tl.x)
, right(rb.x)
, top(tl.y)
, bottom(rb.y)
{}

template<class T>
inline bool RectT<T>::isIn(const Vector2<T>& p) const
{
  return isIn(p.x, p.y);
}

template<class T>
inline bool RectT<T>::isIn(T x, T y) const
{
  return
    ( x <= right  ) &&
    ( x >= left   ) &&
    ( y >= top    ) &&
    ( y <= bottom );
}

template<class T>
inline RectT<T>& RectT<T>::Cover(const Vector2<T>& pt)
{
  right  = Max(pt.x, right);
  left   = Min(pt.x, left);
  bottom = Max(pt.y, bottom);
  top    = Min(pt.y, top);
  return *this;
}

template<class T>
inline RectT<T>& RectT<T>::Cover(const RectT<T>& r)
{
  right  = Max(r.RB().x, right);
  left   = Min(r.TL().x, left);
  bottom = Max(r.RB().y, bottom);
  top    = Min(r.TL().y, top);
  return *this;
}

template<class T>
inline RectT<T>& RectT<T>::Validate()
{
  if(left > right ) std::swap(left, right);
  if(top  > bottom) std::swap(top,  bottom);
  return *this;
}

template<class T>
inline bool RectT<T>::isValid() const
{
  return (right > left) && (bottom > top);
}

template<class T>
inline bool RectT<T>::isIntertsect(const RectT<T>& rect) const
{
  return
    Max(left, rect.left) <= Min(right,  rect.right) &&
    Max(top,  rect.top ) <= Min(bottom, rect.bottom);
}

typedef RectT<f32> Rectf;
typedef RectT<i32> Recti32;
typedef RectT<i16> Recti16;

//////////////////////////////////////////////////////////////////////////
// 3D

const float ONE_THIRD = 1.0f / 3.0f;
const float TWO_THIRD = 2.0f / 3.0f;

template <class T>
struct Sphere
{
  Vector3<T> o;
  T          r;
};

template <class T>
struct Ray
{
  Vector3<T> o;
  Vector3<T> d;
};

typedef Sphere<float> Spheref;
typedef Ray<float>    Rayf;

 // $ can use two v3 instead of ray
template <class T>
inline bool IntersectRaySphere(const Ray<T> &ray, const Sphere<T> &sphere, float* res)
{
  const Vector3<T> dst = ray.o - sphere.o;
  const float B = dst.dotProduct(ray.d);
  const float C = dst.dotProduct(dst) - sphere.r * sphere.r;
  const float D = B*B - C;
  if(D < 0)
    return false;
  if(res)
    *res = -B - sqrt(D);
  return true;
}

//--------------------------------------------------------------------------------------
// Given a ray origin (orig) and direction (dir), and three vertexes of a triangle, this
// function returns TRUE and the interpolated texture coordinates if the ray intersects 
// the triangle
//--------------------------------------------------------------------------------------
template <class T>
inline bool IntersectTriangle(
                              const Vector3<T>& orig, const Vector3<T>& dir,
                              const Vector3<T>& v0,   const Vector3<T>& v1, const Vector3<T>& v2)
{
  // Find vectors for two edges sharing vert0
  Vector3<T> edge1 = v1 - v0;
  Vector3<T> edge2 = v2 - v0;

  // Begin calculating determinant - also used to calculate U parameter
  Vector3<T> pvec = dir.crossProduct(edge2);

  // If determinant is near zero, ray lies in plane of triangle
  T det = edge1.dotProduct(pvec);

  Vector3<T> tvec;
  if( det > 0 )
    tvec = orig - v0;
  else
  {
    tvec = v0 - orig;
    det = -det;
  }

  if( det < 0.0001f )
    return false;

  // Calculate U parameter and test bounds
  float u = tvec.dotProduct(pvec);
  if( u < 0.0f || u > det )
    return false;

  // Prepare to test V parameter
  Vector3<T> qvec = tvec.crossProduct(edge1);

  // Calculate V parameter and test bounds
  T v = dir.dotProduct(qvec);
  if( v < 0.0f || u + v > det )
    return false;

  return true;
}

NAMESPACE_END(mt)

#endif // _mt_base_h 
