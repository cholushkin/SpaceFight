#ifndef _INPUT_SYSTEM_H_
#define _INPUT_SYSTEM_H_

#include "entt/entity/registry.hpp"

// This class will take user input and pass it to appropriate controllers
class InputSystem
{
    struct KeyStatusMap
    {
        bool KeyW;
    };
public:
    InputSystem() = default;
    ~InputSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);
private: 
    KeyStatusMap m_keyStatusMap;

    void ProcessInput();
};

#endif