#include "vps.h"
#include <algorithm> 

namespace vp
{
    UidGenerator VerletPhysicsSystem::sUidGenerator;
    const uint32_t VerletAgent::s_invalidEntity = 0xffffffff;

    VerletAgent::VerletAgent(const Descriptor &desc)
    {
        pos = desc.pos;
        radius = desc.radius;
        deltaPos = desc.velocity;
        acceleration = zero_v2f;
        m_isStatic = desc.m_isStatic;
        m_isSensor = desc.m_isSensor;
        friction = desc.friction;
        mUid = VerletPhysicsSystem::getUid();
        m_entity = s_invalidEntity;
    }

    VerletAgent::VerletAgent()
    {
        deltaPos = zero_v2f;
        acceleration = zero_v2f;
        m_entity = s_invalidEntity;
        mUid = VerletPhysicsSystem::getUid();
    }

    VerletAgent::~VerletAgent()
    {
        VerletPhysicsSystem::freeUid(mUid);
    }

    void VerletAgent::Push(const mt::v2f &delta)
    {
        if (m_isStatic)
            return;
        deltaPos += delta;
        pos += delta;
    }

    void VerletAgent::Move(float /*timeStep*/, float hsqtimeStep, float maxVel_x_timeStep)
    {
        if (m_isStatic)
            return;
        deltaPos += acceleration * hsqtimeStep;
        if (deltaPos.lengthSq() > maxVel_x_timeStep*maxVel_x_timeStep)
            deltaPos = deltaPos.normal() * maxVel_x_timeStep;
        deltaPos *= 0.999f;
        deltaPos *= (1.0f - friction);

        pos += deltaPos;
        acceleration = zero_v2f;
    }

    void VerletAgent::ApplyAcceleration(const mt::v2f& acc)
    {
        acceleration += acc;
    }



    VerletAgent _aabbProxyVerletAgent;
    // -------------- VerletParticleSystem
    VerletPhysicsSystem::VerletPhysicsSystem(const AABB& _bounds)
    {
        bounds = _bounds;

        mParticleCount = 0;
        prevTimeStep = 1e7f;
        mCollListener = NULL;
    }

    void VerletPhysicsSystem::clear()
    {
        mParticleCount = 0;
        prevTimeStep = 1e7f;
        m_vObstacles.clear();
        mColls.clear();
    }

    void VerletPhysicsSystem::AddParticle(VerletAgent* particle)
    {
        sapQueue[mParticleCount++] = particle;
    }

    void VerletPhysicsSystem::RemoveParticle(VerletAgent* particle)
    {
        for (int i = 0; i < mParticleCount; ++i)
            if (sapQueue[i] == particle)
            {
                sapQueue[i] = sapQueue[mParticleCount - 1];
                --mParticleCount;
                return;
            }
    }

    int VerletPhysicsSystem::GetParticlesCount() const
    {
        return mParticleCount;
    }

    int VerletPhysicsSystem::ParticleSortFunc(const void *n1, const void *n2)
    {
        const VerletAgent *p1 = *((const VerletAgent **)n1);
        const VerletAgent *p2 = *((const VerletAgent **)n2);
        return (p1->pos.x - p1->radius > p2->pos.x - p2->radius) ? 1 : -1;
    }


    void VerletPhysicsSystem::ProcessTime(float timeStep)
    {
        const float maxVel = 600.0f;
        const float maxVel_x_timeStep = maxVel * timeStep;
        for (int i = 0; i < mParticleCount; i++)
            if (!sapQueue[i]->m_isStatic)
                sapQueue[i]->Move(timeStep, timeStep * timeStep * 0.5f, maxVel_x_timeStep);

        qsort(&sapQueue, mParticleCount, sizeof(VerletAgent*), ParticleSortFunc);
        mColls.clear();
        for (int iter = 0; 1 != iter; ++iter)
        {
            for (int i = 0; mParticleCount != i; ++i)
            {
                VerletAgent& pi = *sapQueue[i];
                const float pi_dist_r = pi.pos.x + pi.radius;
                //const float pi_dist_l = pi.pos.x - pi.radius;
                for (int j = i + 1; mParticleCount != j; ++j)
                {
                    if ((sapQueue[j]->pos.x - sapQueue[j]->radius > pi_dist_r))
                        break;
                    if (pi.m_isStatic && sapQueue[j]->m_isStatic)
                        continue;
                    if (isCollidePossible(pi, *sapQueue[j]) && Collide(pi, *sapQueue[j]))
                        mColls.insert(Collision(&pi, sapQueue[j]));
                }
            }

            if (bounds.isValid())
                for (int i = 0; i < mParticleCount; ++i)
                    if (CollideBorder(bounds, *sapQueue[i]))
                        mColls.insert(Collision(&_aabbProxyVerletAgent, sapQueue[i]));

            // Obstacles
            for (vObstacles::const_iterator it = m_vObstacles.begin(); m_vObstacles.end() != it; ++it) {
                if ((*it)->m_bb.isValid())
                {
                    for (int i = 0; i < mParticleCount; ++i)
                        if (isCollidePossible((*it)->m_particle, *sapQueue[i]) && CollideSolid((*it)->m_bb, *sapQueue[i]))
                            mColls.insert(Collision(&(*it)->m_particle, sapQueue[i]));
                }
                else if ((*it)->m_particle.isValid())
                {
                    for (int i = 0; i < mParticleCount; ++i)
                        if (isCollidePossible((*it)->m_particle, *sapQueue[i]) && CollideSolid((*it)->m_particle, *sapQueue[i]))
                            mColls.insert(Collision(&(*it)->m_particle, sapQueue[i]));
                }
            }
        }
        prevTimeStep = timeStep;

        // collision callbacks
        if (mCollListener)
            for (std::set<Collision>::iterator i = mColls.begin(); i != mColls.end(); ++i)
                mCollListener->OnCollide(*i);
    }

    void VerletPhysicsSystem::AddObstacle(const Obstacle* obst)
    {
        //EASSERT(m_vObstacles.end() == std::find(m_vObstacles.begin(), m_vObstacles.end(), obst));
        if (m_vObstacles.end() == std::find(m_vObstacles.begin(), m_vObstacles.end(), obst))
            m_vObstacles.push_back(obst);
    }

    void VerletPhysicsSystem::RemoveObstacle(const Obstacle* obst)
    {
        EASSERT(obst != NULL);
        //EASSERT(m_vObstacles.end() != std::find(m_vObstacles.begin(), m_vObstacles.end(), obst));
        if (std::find(m_vObstacles.begin(), m_vObstacles.end(), obst) == m_vObstacles.end())
            return;
        m_vObstacles.erase(std::remove(m_vObstacles.begin(), m_vObstacles.end(), obst), m_vObstacles.end());
    }


    bool VerletPhysicsSystem::isCollidePossible(const VerletAgent& p1, const VerletAgent& p2) const
    {
        const Filter& filterA = p1.m_Filter;
        const Filter& filterB = p2.m_Filter;
        if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
            return filterA.groupIndex > 0;
        return (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
    }


    bool VerletPhysicsSystem::CollideBorder(const AABB& box, VerletAgent &p)
    {
        if (p.pos.x < box.TL().x + p.radius)
        {
            if (!p.m_isSensor)
            {
                p.pos.x = box.TL().x + p.radius;
                if (p.deltaPos.x < 0.0f)
                    p.deltaPos.x = fabs(p.deltaPos.x) * 0.1f;
            }
            return true;
        }
        if (p.pos.x > box.RB().x - p.radius)
        {
            if (!p.m_isSensor)
            {
                p.pos.x = box.RB().x - p.radius;
                if (p.deltaPos.x > 0.0f)
                    p.deltaPos.x = -fabs(p.deltaPos.x) * 0.1f;
            }
            return true;
        }
        if (p.pos.y < box.TL().y + p.radius)
        {
            if (!p.m_isSensor)
            {
                p.pos.y = box.TL().y + p.radius;
                if (p.deltaPos.y < 0.0f)
                    p.deltaPos.y = fabs(p.deltaPos.y) * 0.1f;
            }
            return true;
        }
        if (p.pos.y > box.RB().y - p.radius)
        {
            if (!p.m_isSensor)
            {
                p.pos.y = box.RB().y - p.radius;
                if (p.deltaPos.y > 0.0f)
                    p.deltaPos.y = -fabs(p.deltaPos.y) * 0.1f;
            }
            return true;
        }
        return false;
    }

    bool VerletPhysicsSystem::CollideSolid(const AABB& _box, VerletAgent &p)
    {
        if (p.m_isStatic)
            return false;

        AABB box(_box);
        box.Expand(p.radius);

        if (!box.isIn(p.pos))
            return false;

        f32 to_l = fabs(p.pos.x - box.TL().x);
        f32 to_r = fabs(p.pos.x - box.RB().x);
        f32 to_t = fabs(p.pos.y - box.TL().y);
        f32 to_b = fabs(p.pos.y - box.RB().y);

        f32 dist = to_l;
        int closest = 0;
        if (to_r < dist) {
            closest = 1;
            dist = to_r;
        }
        if (to_t < dist) {
            closest = 2;
            dist = to_t;
        }
        if (to_b < dist) {
            closest = 3;
            dist = to_b;
        }

        if (0 == closest)
        {
            if (p.m_isSensor)
                return true;
            p.pos.x = box.TL().x;
            if (p.deltaPos.x < 0.0f)
                p.deltaPos.x = fabs(p.deltaPos.x) * 0.1f;
            return true;
        }
        else if (1 == closest)
        {
            if (p.m_isSensor)
                return true;
            p.pos.x = box.RB().x;
            if (p.deltaPos.x > 0.0f)
                p.deltaPos.x = -fabs(p.deltaPos.x) * 0.1f;
            return true;
        }
        else if (2 == closest)
        {
            if (p.m_isSensor)
                return true;
            p.pos.y = box.TL().y;
            if (p.deltaPos.y < 0.0f)
                p.deltaPos.y = fabs(p.deltaPos.y) * 0.1f;
            return true;
        }
        else if (3 == closest)
        {
            if (p.m_isSensor)
                return true;
            p.pos.y = box.RB().y;
            if (p.deltaPos.y > 0.0f)
                p.deltaPos.y = -fabs(p.deltaPos.y) * 0.1f;
            return true;
        }
        return false;
    }

    bool VerletPhysicsSystem::Collide(VerletAgent &p1, VerletAgent &p2)
    {
        mt::v2f delta = p1.pos - p2.pos;
        f32 quadlen = delta.lengthSq();
        f32 maxDist = (p1.radius + p2.radius);
        if (quadlen >= maxDist * maxDist)
            return false;

        if (quadlen < 0.0001f)
            return false;

        f32 len = sqrtf(quadlen);
        mt::v2f dir = delta / len;

        f32 ratio = p1.radius / maxDist;
        f32 dist = (maxDist - len) * 0.25f;

        if (p1.m_isSensor || p2.m_isSensor)
            return true;

        p1.Push(dir * (dist * (1.0f - ratio)));
        p2.Push(-dir * (dist * (ratio)));
        return true;
    }

    bool VerletPhysicsSystem::CollideSolid(const VerletAgent& p1, VerletAgent& p2)
    {
        mt::v2f delta = p1.pos - p2.pos;
        f32 quadlen = delta.lengthSq();
        f32 maxDist = (p1.radius + p2.radius);
        if (quadlen >= maxDist * maxDist)
            return false;

        if (quadlen < 0.0001f)
            return false;

        f32 len = sqrtf(quadlen);
        mt::v2f dir = delta / len;

        f32 ratio = p1.radius / maxDist;
        f32 dist = (maxDist - len) * 0.25f;

        if (!p2.m_isSensor)
            p2.Push(-dir * (dist * (ratio)));
        return true;
    }
}