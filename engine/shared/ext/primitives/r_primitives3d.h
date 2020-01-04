#ifndef r_primitives3d_h 
#define r_primitives3d_h 

#include "core/render/r_render.h"
#include "core/res/res_base.h"
#include "core/math/mt_base.h"
#include "ext/math/mt_colors.h"

NAMESPACE_BEGIN(r)

//////////////////////////////////////////////////////////////////////////
// 3D

// ... Material3D
// [ ] FVF
// [ ] Normals

//////////////////////////////////////////////////////////////////////////
// Coordinates

struct Coordinates
{
  Coordinates() : m_bisMValid(false) {}

  const mt::v3f&         GetPos() const { return pos; }
  const mt::Quaternionf& GetRot() const { return rot; }

  void SetPos(const mt::v3f&         p) {pos = p; m_bisMValid = false; }
  void SetRot(const mt::Quaternionf& q) {rot = q; m_bisMValid = false; }

  const mt::Matrix4f& GetMatrix() const
  {
    if(!m_bisMValid)
    {
      m_local = mt::Matrix4f::createTranslation(pos.x, pos.y, pos.z) * rot.transform();
      m_bisMValid = true;
    }
    return m_local;
  }

  mt::v3f pos;
  mt::Quaternionf rot;

   // cache
  mutable bool m_bisMValid; // for local only
  mutable mt::Matrix4f m_local;

  mutable mt::Matrix4f m_transformed; //must not be there

};

//////////////////////////////////////////////////////////////////////////
// Mesh

bool LoadMesh(Mesh& mesh, const char* szName);

//////////////////////////////////////////////////////////////////////////
// HierarhicalMesh

class HierarhicalMesh
{
public:
  HierarhicalMesh();
  ~HierarhicalMesh();

  // Ink: let it be. default values
  void Draw(Render& r, const mt::Matrix4f& pos, const mt::Matrix4f& cam, const Coordinates* coord = NULL, const bool* visible = NULL) const;
  void UpdateHierarchy(Coordinates* coord = NULL);

  Mesh*        m_meshes; // $$$ Ink: there comes hack - meshes have NULL vb pointers. will change later
  i16*         m_hierarchy;
  Coordinates* m_coord; // these can be animated
  u16          m_nmeshes;
  Vertex3f*    m_v;
  u16*         m_i;
private:
  HierarhicalMesh(const HierarhicalMesh&);
  HierarhicalMesh& operator=(const HierarhicalMesh&);
};

// $$$ Ink: temporary
const HierarhicalMesh* GetHMesh(res::ResID id, res::ResourcesPool& pool, Render& r);
bool LoadHMesh(HierarhicalMesh& mesh, const char* szName, Render&r, res::ResourcesPool& p, Material::eBlendMode blend);
void ApplyMaterial(HierarhicalMesh& mesh, res::ResID texture, Material::eBlendMode blend, Render&r, res::ResourcesPool& p);

NAMESPACE_END(r)

#endif // r_primitives3d_h 
