#ifndef level_h__
#define level_h__
#include "core/common/com_types.h"
#include "physicssystem.h"

class Level final
{
    DENY_COPY(Level)
private:
    class GenerationOptions
    {
    public:
        GenerationOptions();
        mt::v2i16 PlanetsAmmount;
        mt::v2i16 EnergyStationAmmount;
        mt::v2i16 AsteroidsAmmount;
    };

public:
    Level(entt::DefaultRegistry& registry, PhysicsSystem& psx);
    void CreateLevelRandom();
    void DeleteEntity(uint32_t&);

private:
    entt::DefaultRegistry&      m_registry;
    PhysicsSystem&              m_physicsSystem;
    GenerationOptions           m_options;
    std::vector<mt::v2f>        m_created;


    void CreatePlayerEntity(int playerID, const mt::v2f& pos);
    void CreatePlanet(const mt::v2f& pos);
};


#endif