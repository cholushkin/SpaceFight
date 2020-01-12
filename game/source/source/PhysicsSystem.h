#ifndef _PHYSICS_SYSTEM_H_
#define _PHYSICS_SYSTEM_H_

#include "entt/entity/registry.hpp"
#include "EntityTypeComponent.h"
#include "vphys/vps.h"

class Level;
struct GameResources;

class PhysicsSystem : public vp::ICollisionListener
{
public:
    PhysicsSystem(entt::DefaultRegistry& registry, Level& level);
    ~PhysicsSystem();

    vp::VerletAgent* AddAgent(const uint32_t& ent, const mt::v2f pos, float friction, float radius, bool isStatic);
    vp::Obstacle* AddObstacle(const uint32_t& ent, const mt::v2f pos);
    void RemoveAgent(const uint32_t& ent);
    void RemoveObstacle(const uint32_t& ent);
    
    void Update(float dt, entt::DefaultRegistry& registry, GameResources& res);

private:
    vp::VerletPhysicsSystem  m_vps; // verlet physics simulation system
    entt::DefaultRegistry&   m_registry;
    Level&                   m_level;

    void OnCollide(const vp::Collision& collision);
};

#endif