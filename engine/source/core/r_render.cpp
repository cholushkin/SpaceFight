#include "core/render/r_render.h"
#include "r_device_gl.h"
#include "core/log/log_log.h"
#include "core/res/res_base.h"

using namespace res;
using namespace mt;
using namespace r;
Context Device::sContext;

//////////////////////////////////////////////////////////////////////////
// Context

Context::Context(const v2i16& pss, const v2i16& uss, eScreenFitMode sfm)
:  m_physScreenSize(pss)
,  m_userScreenSize(uss)
,  m_fit(sfm)
{
  ComputeVirtualScreen();
}

Context::Context()
:  m_physScreenSize(v2i16())
,  m_userScreenSize(v2i16())
,  m_fit(sfNoScale)
,  m_virtScreenOffset(v2i16())
,  m_virtScreenSize(v2i16())
,  m_usrAspectRatio(0)
{
}

void Context::SetPhysScreenSize(const v2i16& pss)
{
  m_physScreenSize = pss;
  ComputeVirtualScreen();
}

void Context::SetUserScreenSize(const v2i16& uss, Context::eScreenFitMode sfm)
{
  m_userScreenSize = uss;
  m_fit = sfm;
  ComputeVirtualScreen();
}

void Context::MapInput( int& x, int& y ) const
{
  f32 kx = (f32)(x-m_virtScreenOffset.x)/(f32)m_virtScreenSize.x;
  f32 ky = (f32)(y-m_virtScreenOffset.y)/(f32)m_virtScreenSize.y;
  x = (int)(m_userScreenSize.x * kx);
  y = (int)(m_userScreenSize.y * ky);
}

bool Context::IsInsideVirtualScreen(const v2i16& pos) const
{
  return !(
    (pos.x<m_virtScreenOffset.x) || (pos.x>m_virtScreenOffset.x+m_virtScreenSize.x)||
    (pos.y<m_virtScreenOffset.y) || (pos.y>m_virtScreenOffset.y+m_virtScreenSize.y)
  );
}

void Context::ComputeVirtualScreen()
{
  m_usrAspectRatio = (f32)m_userScreenSize.x/(f32)m_userScreenSize.y;
  if(m_fit==sfFitFullScreen)
  {
    FitByWidth();
    if(m_virtScreenSize.y > m_physScreenSize.y)
      FitByHeight();
  } 
  else if (m_fit==sfFitByScreenWidth)
    FitByWidth();
  else if (m_fit==sfFitByScreenHeight)
    FitByHeight();
  else if (m_fit==sfStretch)
    FitStretch();
  else if (m_fit==sfNoScale)
    FitNoScale();

  // center virtual screen inside phys screen ( todo: passable parameters for aligning (vert & horiz) )
  m_virtScreenOffset.x = (i16)( (m_physScreenSize.x - m_virtScreenSize.x) * 0.5f );
  m_virtScreenOffset.y = (i16)( (m_physScreenSize.y - m_virtScreenSize.y) * 0.5f );
}

void Context::FitByWidth()
{
  m_virtScreenSize.x = m_physScreenSize.x;
  m_virtScreenSize.y = m_virtScreenSize.x / m_usrAspectRatio;
}

void Context::FitByHeight()
{
  m_virtScreenSize.y = m_physScreenSize.y;
  m_virtScreenSize.x = m_virtScreenSize.y * m_usrAspectRatio;
}

void Context::FitStretch()
{
  m_virtScreenSize.x = m_physScreenSize.x;
  m_virtScreenSize.y = m_physScreenSize.y;
}

void Context::FitNoScale()
{
  m_virtScreenSize.x = m_userScreenSize.x;
  m_virtScreenSize.y = m_userScreenSize.y;
}

void Context::clampToUsrScreen(v2i16& coord) const
{
  coord.x = Clamp<i16>(coord.x, m_virtScreenOffset.x, m_virtScreenOffset.x + (i16)m_virtScreenSize.x);
  coord.y = Clamp<i16>(coord.y, m_virtScreenOffset.y, m_virtScreenOffset.y + (i16)m_virtScreenSize.y);
}

v2i16 Context::UserToPhysical(int x, int y) const
{
  v2i16 user = GetUserScreenSize();
  if(0 == user.x || 0 == user.y)
    return v2i16((i16)x, (i16)y);
  v2i16 offs = GetVirtScreenOffset();
  v2i16 virt = GetVirtScreenSize();
  v2i16 phys = GetPhysScreenSize();
  int fx = x * virt.x / user.x + offs.x;
  int fy = y * virt.y / user.y + offs.y;
  return v2i16((i16)fx, (i16)fy); 
}

//////////////////////////////////////////////////////////////////////////
// Render

Render::Render()
: m_vsize_curr(0)
, m_isize_curr(0)
{
}

void Render::PostQuad(const v2f coords[4], const v2f uv[4], const u32 colors[4], const Material& m)
{
  if((m != m_dip.material) ||
    (m_vsize_curr + 4 >= s_vsize_max) || 
    (m_isize_curr + 6 >= s_isize_max) ||
    (m_dip.type!=Device::ptTriangles))
  {
    FlushDIPs();
    m_dip.material = m;
    m_dip.type = Device::ptTriangles;
  }

  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 0;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 1;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 2;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 0;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 2;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 3;

  m_vb[m_vsize_curr++] = Vertex2f(coords[0], uv[0], colors[0]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[1], uv[1], colors[1]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[2], uv[2], colors[2]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[3], uv[3], colors[3]);

  m_dip.primcnt += 2;
}

void Render::PostTri(const v2f coords[3], const v2f uv[3], const u32 colors[3], const Material& m)
{
  if((m != m_dip.material) ||
    (m_vsize_curr + 3 >= s_vsize_max) || 
    (m_isize_curr + 3 >= s_isize_max) ||
    (m_dip.type!=Device::ptTriangles))
  {
    FlushDIPs();
    m_dip.material = m;
    m_dip.type = Device::ptTriangles;
  }

  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 0;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 1;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 2;

  m_vb[m_vsize_curr++] = Vertex2f(coords[0], uv[0], colors[0]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[1], uv[1], colors[1]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[2], uv[2], colors[2]);

  m_dip.primcnt += 1;
}

void Render::PostLine(const v2f coords[2], const u32 colors[2], const Material& m)
{
  if((m != m_dip.material) || (m_vsize_curr + 2 >= s_vsize_max) || (m_isize_curr + 2 >= s_isize_max) || (m_dip.type!=Device::ptLines))
  {
    FlushDIPs();
    m_dip.material = m;
    m_dip.type = Device::ptLines;
  }

  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 0;
  m_ib[m_isize_curr++] = (u16)m_vsize_curr + 1;

  m_vb[m_vsize_curr++] = Vertex2f(coords[0], v2f(0,0), colors[0]);
  m_vb[m_vsize_curr++] = Vertex2f(coords[1], v2f(0,0), colors[1]);
  m_dip.primcnt += 1;
}

void Render::RenderMesh(const Mesh& m, const Matrix4f& pos, const Matrix4f& cam)
{
  FlushDIPs();
  // setup matrices
  m_device.SetModel(pos);
  m_device.SetProjection(cam);
  for(u32 im = 0; m.m_nmeshes != im; ++im)
    m_device.Draw(Device::ptTriangles, m.m_meshes[im].m_mat, &m.m_meshes[im].m_v[0], &m.m_meshes[im].m_i[0], &m.m_meshes[im].m_i[0] != NULL ? m.m_meshes[im].m_ni / 3 : m.m_meshes[im].m_nv / 3);
}

void Render::Clip(const Recti32& r)
{
  FlushDIPs();
  m_device.Clip(r);
}

void Render::SetColorMask(bool r, bool g, bool b, bool a)
{
  FlushDIPs();
  m_device.SetColorMask(r, g, b, a);
}

void Render::SetModel(const Matrix4f& pos)
{
  FlushDIPs();
  m_device.SetModel(pos);
}

void Render::SetProjection(const Matrix4f& cam)
{
  FlushDIPs();
  m_device.SetProjection(cam);
}

v2i16 Render::GetScreenSize() const
{
  return m_device.GetScreenSize();
}

void Render::FlushDIPs()
{
  if(0 == m_dip.primcnt)
    return;
  m_device.Set2D();
  m_device.Draw(m_dip.type, m_dip.material, &m_vb[0], &m_ib[0], m_dip.primcnt);
  m_vsize_curr = 0;
  m_isize_curr = 0;
  m_dip.primcnt = 0;
}

void Render::Clear()
{
  m_device.BeginFrame();
}

void Render::SetClearColor(u32 clr)
{
  m_device.SetClearColor(clr);
}

void Render::Present()
{
  FlushDIPs();
  m_device.EndFrame();
}

bool Render::Init()
{
  return m_device.Init();
}
void Render::Release()
{
  m_vsize_curr = 0;
  m_device.Release();
}

const Texture* Render::GetTexture(ResID ID, ResourcesPool& p)
{
  return m_device.GetTexture(ID, p);
}

//////////////////////////////////////////////////////////////////////////
// Device - hardware specific

//////////////////////////////////////////////////////////////////////////
//// Texture


Texture::~Texture()
{
  SAFE_DELETE(m_pimpl);
}

void Texture::Set(u32 stage) const
{
  m_pimpl->Set(stage);
}

const v2u16& Texture::GetSize() const
{
  return m_pimpl->GetSize();
}

const v2u16& Texture::GetRect() const
{
  return m_pimpl->GetRect();
}


//////////////////////////////////////////////////////////////////////////
// Mesh

Mesh::~Mesh()
{
  SAFE_DELETE_ARRAY(m_meshes);
  SAFE_DELETE_ARRAY(m_v);
  SAFE_DELETE_ARRAY(m_i);
}

//////////////////////////////////////////////////////////////////////////
//// Device

void Device::BeginFrame()
{
  EASSERT(m_impl);
  m_impl->BeginFrame();
}

void Device::EndFrame()
{
  EASSERT(m_impl);
  m_impl->EndFrame();
}

void Device::Clip(const Recti32& r)
{
  EASSERT(m_impl);
  m_impl->Clip(r);
}

void Device::SetColorMask(bool r, bool g, bool b, bool a)
{
  m_impl->SetColorMask(r, g, b, a);
}

void Device::Set2D()
{
  EASSERT(m_impl);
  m_impl->Set2D();
}

void Device::SetClearColor (u32 clr)
{
  EASSERT(m_impl);
  m_impl->SetClearColor(clr);
}

void Device::Draw(ePrimitiveType prim, const Material& m, const Vertex2f* v, const u16* idx, u32 PrimCount)
{
  EASSERT(m_impl);
  if(0 == PrimCount)
    return;
  m_impl->Draw(prim, m, v, idx, PrimCount);
}

void Device::Draw(ePrimitiveType prim, const Material& m, const Vertex3f* v, const u16* idx, u32 PrimCount)
{
  EASSERT(m_impl);
  if(0 == PrimCount)
    return;
  m_impl->Draw(prim, m, v, idx, PrimCount);
}

void Device::SetModel(const Matrix4f& pos)
{
  EASSERT(m_impl);
  m_impl->SetModel(pos);
}

void Device::SetProjection (const Matrix4f& cam)
{
  EASSERT(m_impl);
  m_impl->SetProjection(cam);
}

const Texture* Device::GetTexture(ResID ID, ResourcesPool& p)
{
  EASSERT(m_impl);
  return m_impl->GetTexture(ID, p);
}

bool Device::Init()
{
  EASSERT(!m_impl);
  m_impl = new DeviceImpl();
  bool isInitOK = m_impl->Init();
  if(isInitOK)
    sContext.SetUserScreenSize(GetScreenSize(), Context::sfStretch);
  return isInitOK;
}

void Device::Release()
{
  EASSERT(m_impl);
  m_impl->Release();
  SAFE_DELETE(m_impl);
}

v2i16 Device::GetScreenSize() const
{
  EASSERT(m_impl);
  return sContext.GetUserScreenSize();
}


