#include "ext/primitives/r_primitives3d.h"
#include "ext/algorithm/alg_arrays.h"
#include "core/io/io_base.h"
#include "core/res/res_base.h"

using namespace res;
using namespace mt;
using namespace alg;

NAMESPACE_BEGIN(r)

//////////////////////////////////////////////////////////////////////////
// Mesh

bool LoadMesh(Mesh& mesh, const char* szName)
{
  io::FileStream f;
  if(!f.Open(szName, true, true))
    return false;
  u16 s = 0;
  f.Read((char*)&s, sizeof(s));
  mesh.m_v = new Vertex3f[s];
  f.Read((char*)&mesh.m_v[0], s * sizeof(Vertex3f));
  for(u16 t = 0; s != t; ++t)
    mesh.m_v[t].uv.y = 1.0f - mesh.m_v[t].uv.y;

  f.Read((char*)&s, sizeof(s));
  mesh.m_nmeshes = s;
  mesh.m_meshes = new SubMesh[s];
  v2i16 vi;
  for(u16 t = 0; s != t; ++t)
  {
    f.Read((char*)&vi, sizeof(vi));
    mesh.m_meshes[t].m_v = &mesh.m_v[0] + vi.x * 3;
    mesh.m_meshes[t].m_nv = vi.y * 3;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
// HierarhicalMesh

HierarhicalMesh::HierarhicalMesh()
: m_meshes(NULL)
, m_hierarchy(NULL)
, m_coord(NULL)
, m_v(NULL)
, m_i(NULL)
, m_nmeshes(0)
{}

HierarhicalMesh::~HierarhicalMesh()
{
  SAFE_DELETE_ARRAY(m_meshes);
  SAFE_DELETE_ARRAY(m_hierarchy);
  SAFE_DELETE_ARRAY(m_coord);
  SAFE_DELETE_ARRAY(m_v);
  SAFE_DELETE_ARRAY(m_i);
}

void HierarhicalMesh::Draw(Render& r, const Matrix4f& view, const Matrix4f& proj, const Coordinates* coord, const bool* visible) const
{
  if(NULL == coord)
    coord = m_coord;
  if(NULL == visible)
  {
    for(u16 t = 0; m_nmeshes != t; ++t)
      r.RenderMesh(m_meshes[t],view * coord[t].m_transformed, proj);
  }
  else
  {
    for(u16 t = 0; m_nmeshes != t; ++t)
      if(visible[t])
        r.RenderMesh(m_meshes[t],view * coord[t].m_transformed, proj);
  }
}

void HierarhicalMesh::UpdateHierarchy(Coordinates* coord)
{
  if(NULL == coord)
    coord = m_coord;
  for(u16 t = 0; m_nmeshes != t; ++t)
  {
    // cache me
    Matrix4f mtx = coord[t].GetMatrix();
    int idx = m_hierarchy[t];
    while(-1 != idx)
    {
      mtx = coord[idx].GetMatrix() * mtx;
      idx = m_hierarchy[idx];
    }
    coord[t].m_transformed = mtx;
  }
}

// $$$ textures loading

extern const Texture* GetFontTexture(const char* szName, ResourcesPool& pool, Render& r);

bool LoadHMesh(HierarhicalMesh& mesh, const char* szName, Render&r, ResourcesPool& p, Material::eBlendMode blend)
{
  io::FileStream f;
  if(!f.Open(szName, true, true))
    return false;

  SAFE_DELETE_ARRAY(mesh.m_meshes);
  SAFE_DELETE_ARRAY(mesh.m_hierarchy);
  SAFE_DELETE_ARRAY(mesh.m_coord);
  SAFE_DELETE_ARRAY(mesh.m_v);
  SAFE_DELETE_ARRAY(mesh.m_i);

  f.Read16(mesh.m_nmeshes);
  mesh.m_meshes     = new Mesh       [mesh.m_nmeshes];
  mesh.m_hierarchy  = new i16        [mesh.m_nmeshes];
  mesh.m_coord      = new Coordinates[mesh.m_nmeshes];

  u16 nv = 0;
  u16 ni = 0;
  f.Read16(nv);
  f.Read16(ni);

  mesh.m_v = new Vertex3f[nv]; // interleave me
  mesh.m_i = new u16[ni];

  f.Read((char*)&mesh.m_v[0], nv * sizeof(Vertex3f));
  f.Read((char*)&mesh.m_i[0], ni * sizeof(u16));

  u16 vi = 0;
  u16 ii = 0;
  for(u16 t = 0; mesh.m_nmeshes != t; ++t)
  {
    f.Read16(mesh.m_meshes[t].m_nmeshes);
    f.Read16(mesh.m_hierarchy[t]);
    f.Read((char*)&mesh.m_coord[t].pos, sizeof(mesh.m_coord[t].pos));
    f.Read((char*)&mesh.m_coord[t].rot, sizeof(mesh.m_coord[t].rot));
    mesh.m_meshes[t].m_meshes = new SubMesh[mesh.m_meshes[t].m_nmeshes];
    for(u16 m = 0; mesh.m_meshes[t].m_nmeshes != m; ++m)
    {
      f.Read16(mesh.m_meshes[t].m_meshes[m].m_nv);
      f.Read16(mesh.m_meshes[t].m_meshes[m].m_ni);
      mesh.m_meshes[t].m_meshes[m].m_v = &mesh.m_v[vi];
      mesh.m_meshes[t].m_meshes[m].m_i = &mesh.m_i[ii];
      vi = vi + mesh.m_meshes[t].m_meshes[m].m_nv;
      ii = ii + mesh.m_meshes[t].m_meshes[m].m_ni;
    }
  }
  EASSERT(nv == vi);
  EASSERT(ni == ii);

  char szTexture[256];
  for(u16 t = 0; mesh.m_nmeshes != t; ++t)
  {
    for(u16 m = 0; mesh.m_meshes[t].m_nmeshes != m; ++m)
    {
      int tidx = 0;
      do
      {
        if(!f.Read(&szTexture[tidx], 1))
        {
          EALWAYS_ASSERT("Error parsing texture filename");
          return false;
        }
        ++tidx;
      }
      while(szTexture[tidx - 1]);
      mesh.m_meshes[t].m_meshes[m].m_mat =  Material(GetFontTexture(szTexture, p, r), blend);;
    }
  }
  return true;
}


void ApplyMaterial(HierarhicalMesh& mesh, ResID texture, Material::eBlendMode blend, Render&r, ResourcesPool& p)
{
  const Material mat(r.GetTexture(texture, p), blend);
  for(u16 t = 0; mesh.m_nmeshes != t; ++t)
    for(u16 m = 0; mesh.m_meshes[t].m_nmeshes != m; ++m)
      mesh.m_meshes[t].m_meshes[m].m_mat = mat;
}

const HierarhicalMesh* GetHMesh(ResID id, ResourcesPool& pool, Render& r)
{
  HierarhicalMesh* res = (HierarhicalMesh*)pool.Get(id);
  if(NULL != res)
    return res;
  const char* szFile = ResourcesPool::GetNameFromID(id);
  if(NULL == szFile)
    return NULL;
  res = new HierarhicalMesh();
  if(!LoadHMesh(*res, szFile, r, pool, Material::bmNone))
  {
    SAFE_DELETE(res);
    return NULL;
  }
  return pool.PutGeneric(id, res);
}

NAMESPACE_END(r)