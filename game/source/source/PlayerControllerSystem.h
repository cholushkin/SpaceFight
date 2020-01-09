#ifndef _PLAYER_CONTROLLER_SYSTEM_H_
#define _PLAYER_CONTROLLER_SYSTEM_H_

#include "InputSystem.h"
#include "level.h"

class PlayerControllerSystem
{
public:
    PlayerControllerSystem(InputSystem& input, Level& level);
    ~PlayerControllerSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);
private: 
    InputSystem& m_inputSystem;
    Level& m_level;

    mt::v2f GetDistanceToClosestEnemy(const uint32_t& from, entt::DefaultRegistry& registry);
};

#endif