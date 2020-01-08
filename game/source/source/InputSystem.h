#ifndef _INPUT_SYSTEM_H_
#define _INPUT_SYSTEM_H_

#include "entt/entity/registry.hpp"
#include "core/math/mt_base.h"

// This class will take user input and pass it to appropriate controllers
class InputSystem
{
    struct KeyStatusMap
    {
        // keyboard section 1
        bool KeyboardP1_UP;
        bool KeyboardP1_LEFT;
        bool KeyboardP1_DOWN;
        bool KeyboardP1_RIGHT;
        bool KeyboardP1_SHOOT;

        // keyboard section 2
        bool KeyboardP2_UP;
        bool KeyboardP2_LEFT;
        bool KeyboardP2_DOWN;
        bool KeyboardP2_RIGHT;
        bool KeyboardP2_SHOOT;

        // keyboard system section 
        bool Keyboard_Apply;
        bool Keyboard_Cancel;
        
    };
public:
    InputSystem() = default;
    ~InputSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);

    mt::v2f GetVectorFromInput(int playerID);
    bool GetShootButton(int playerID);

private: 
    KeyStatusMap m_keyStatusMap;

    void ProcessInput();
};

#endif