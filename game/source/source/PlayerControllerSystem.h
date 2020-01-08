#ifndef _PLAYER_CONTROLLER_SYSTEM_H_
#define _PLAYER_CONTROLLER_SYSTEM_H_

#include "InputSystem.h"

class PlayerControllerSystem
{
public:
    PlayerControllerSystem(InputSystem& input);
    ~PlayerControllerSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);
private: 
    InputSystem& m_inputSystem;
};

#endif