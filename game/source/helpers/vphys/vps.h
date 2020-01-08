#ifndef vps_h__
#define vps_h__

#include "core/math/mt_base.h"
#include "core/math/mt_vmath.h"
#include "core/common/com_misc.h"

#include "math.h"
#include <vector>
#include "uid_generator/uidgenerator.h"
#include <set>

namespace vp
{
    const static  mt::v2f zero_v2f = mt::v2f(0, 0);
    const static  mt::v2f xAxis = mt::v2f(1, 0);
    const static  mt::v2f yAxis = mt::v2f(0, 1);


    struct Filter
    {
        Filter() :categoryBits(0x0001), maskBits(0xFFFF), groupIndex(0) {}
        u16 categoryBits;     // The collision category bits. Normally you would just set one bit.
        u16 maskBits;         // The collision mask bits. This states the categories that this shape would accept for collision.
        i16 groupIndex;       // Collision groups allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision group. Non-zero group filtering always wins against the mask bits.
    };


    struct VerletAgent;
    class IActor {
    public:
        virtual ~IActor() {}
        virtual void OnCollide(const VerletAgent* agent) = 0;
    };

    struct VerletAgent
    {
        //DENY_COPY(VerletAgent);
    public:
        struct Descriptor
        {
            Descriptor(const mt::v2f& _pos, float _radius = 2.0f, float _friction = 0.0f, const mt::v2f &_velocity = zero_v2f)
                : pos(_pos)
                , m_isStatic(false)
                , m_isSensor(false)
                , friction(_friction)
                , velocity(_velocity)
                , radius(_radius) {}

            Descriptor()
                : pos(zero_v2f)
                , m_isStatic(false)
                , m_isSensor(false)
                , friction(0.0f)
                , velocity(zero_v2f)
                , radius(2.0f) {}

            mt::v2f  pos;
            mt::v2f  velocity;
            f32  radius;
            f32  friction;
            bool m_isStatic;
            bool m_isSensor;
        };

        VerletAgent(const Descriptor &desc);

        VerletAgent();

        ~VerletAgent();

        void Push(const mt::v2f &delta);

        void Move(float /*timeStep*/, float hsqtimeStep, float maxVel_x_timeStep);

        void ApplyAcceleration(const mt::v2f& acc);

        f32  radius;
        f32  friction;
        mt::v2f  pos;
        mt::v2f  deltaPos;
        mt::v2f  acceleration;
        u32  mUid;
        bool m_isStatic;
        bool m_isSensor;
        Filter m_Filter;
        IActor* m_actor;
        bool isValid() const { return radius > 0.0f; }
    };

    typedef mt::Rectf AABB;

    struct Obstacle
    {
        DENY_COPY(Obstacle);
    public:
        Obstacle()
            : m_particle(VerletAgent::Descriptor(zero_v2f, 0.0f))
        {}

        Obstacle(const mt::Rectf& r)
        {
            m_bb = r;
            m_particle.radius = 0;
        }
        Obstacle(const mt::v2f& pos, f32 r)
        {
            m_particle.radius = r;
            m_particle.m_isStatic = true;
            m_particle.pos = pos;
        }
        AABB            m_bb;
        VerletAgent     m_particle;
    };


    struct Collision
    {
        Collision(const VerletAgent* p1, const VerletAgent* p2) :mP1(p1), mP2(p2) { cookUID(); }
        const VerletAgent* mP1;
        const VerletAgent* mP2;

        bool operator<(const Collision& right) const { return mUID < right.mUID; }
    private:
        void cookUID()
        {
            EASSERT(mP1 != NULL);
            EASSERT(mP2 != NULL);
            EASSERT(mP1 != mP2);
            EASSERT(mP1->mUid != mP2->mUid);

            mUID = (u64(mt::Min(mP1->mUid, mP2->mUid)) << 32) + mt::Max(mP1->mUid, mP2->mUid);
        }
        u64 mUID; // for sorting
    };

    class ICollisionListener
    {
    public:
        virtual ~ICollisionListener() {};
        virtual void OnCollide(const Collision& collision) = 0;
    };

    class VerletPhysicsSystem
    {
    public:
        VerletPhysicsSystem(const AABB& _bounds);
        void clear();
        void AddParticle(VerletAgent* particle);
        void RemoveParticle(VerletAgent* particle);
        int  GetParticlesCount() const;
        void ProcessTime(float timeStep);

        void AddObstacle(const Obstacle* obst);
        void RemoveObstacle(const Obstacle* obst);
        void SetCollisionListener(ICollisionListener* colListener) { mCollListener = colListener; }
        bool isCollidePossible(const VerletAgent& p1, const VerletAgent& p2) const; // using filters    

        static u32 getUid() { return sUidGenerator.getUid(); }
        static void freeUid(const u32& uid) { sUidGenerator.freeUid(uid); }

    private:
        static int ParticleSortFunc(const void *n1, const void *n2);

    private:
        static UidGenerator   sUidGenerator;
        static const u32      sMAX_PARTICLES = 1000;

        VerletAgent*          sapQueue[sMAX_PARTICLES];
        int                   mParticleCount;
        f32                   prevTimeStep;
        AABB                  bounds;
        typedef std::vector<const Obstacle*> vObstacles;
        vObstacles            m_vObstacles;

        static bool CollideBorder(const AABB&, VerletAgent& p);
        static bool CollideSolid(const AABB&, VerletAgent& p);
        static bool CollideSolid(const VerletAgent& p1, VerletAgent& p2);
        static bool Collide(VerletAgent&       p1, VerletAgent& p2);


        std::set<Collision> mColls;
        ICollisionListener* mCollListener;
    };
}// namespace
#endif // vps_h__
