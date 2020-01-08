#ifndef _PHYSICS_SYSTEM_H_
#define _PHYSICS_SYSTEM_H_

#include "entt/entity/registry.hpp"
#include "vphys/vps.h"

class PhysicsSystem : public vp::ICollisionListener
{
public:
    PhysicsSystem(entt::DefaultRegistry& registry);
    ~PhysicsSystem();

    vp::VerletAgent* AddAgent(const uint32_t& ent, const mt::v2f pos);
    void RemoveAgent(const uint32_t& ent);


    void Update(float dt, entt::DefaultRegistry& registry);
private: 
    vp::VerletPhysicsSystem  m_vps; // verlet physics simulation system
    entt::DefaultRegistry&   m_registry;


    void OnCollide(const vp::Collision& collision);
};

#endif