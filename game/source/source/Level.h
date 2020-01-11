#ifndef level_h__
#define level_h__
#include "core/common/com_types.h"
#include "physicssystem.h"


struct SessionContext;


typedef mt::v2i16 Range;

class Level final
{
    DENY_COPY(Level)
private:
    class GenerationOptions
    {
    public:
        GenerationOptions();
        Range PlanetsAmmount;
        Range EnergyStationAmmount;
        Range EnergyPickupsAmmount;
        Range AsteroidsAmmount;
    };

public:
    Level(entt::DefaultRegistry& registry, PhysicsSystem& psx, const SessionContext& sessionContext);
    void CreateLevelRandom();
    void DeleteEntity(uint32_t&);

    void CreatePlasmaBullet(const mt::v2f& pos, const mt::v2f& speed);
    void CreateEnergyPickup(const mt::v2f& pos, const mt::v2f& speed);

private:
    entt::DefaultRegistry&                      m_registry;
    PhysicsSystem&                              m_physicsSystem;
    GenerationOptions                           m_options;
    std::vector<mt::v2f>                        m_created;
    const SessionContext&                             m_sessionContext;

    void CreatePlayerEntity(int playerID, const mt::v2f& pos);
    void CreatePlanet(const mt::v2f& pos);
    bool IsHit(const mt::v2f& pos);
};


#endif