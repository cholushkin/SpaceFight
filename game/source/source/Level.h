#ifndef level_h__
#define level_h__
#include "core/common/com_types.h"
#include "physicssystem.h"
#include "ext/math/mt_random.h"


struct SessionContext;

typedef mt::v2i16 Range;
class RandomHelper
{
public:
    static mt::fast_random_generator s_rnd;

    inline static int RndRange(int min, int max) // [min, max]
    {
        return min + s_rnd.rand() % ((max + 1) - min);
    }

    inline static int RndRange(const Range& range)
    {
        return RndRange(range.x, range.y);
    }

    inline static mt::v2f RndPointOnCircle(const mt::v2f& center, float radius)
    {
        mt::v2f result;
        float angle = static_cast<float>(s_rnd.frand() * M_2PI);
        mt::sin_cos_low(angle, result.x, result.y);
        return center + result * radius;
    }
};


// todo: as a system
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

    void CreatePlasmaBullet(const mt::v2f& pos, const mt::v2f& direction);
    void CreateEnergyPickup(const mt::v2f& pos, const mt::v2f& speed);

private:
    entt::DefaultRegistry&                      m_registry;
    PhysicsSystem&                              m_physicsSystem;
    GenerationOptions                           m_options;
    std::vector<mt::v2f>                        m_created;
    const SessionContext&                       m_sessionContext;

    void CreatePlayerEntity(int playerID, const mt::v2f& pos);
    void CreatePlanet(const mt::v2f& pos);
    void CreateEnergyStation(const mt::v2f& pos);
    void CreateBG(const mt::v2f& pos);
    bool IsHit(const mt::v2f& pos);
};


#endif