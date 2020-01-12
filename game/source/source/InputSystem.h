#ifndef _INPUT_SYSTEM_H_
#define _INPUT_SYSTEM_H_

#include "entt/entity/registry.hpp"
#include "core/math/mt_base.h"

class InputSystem
{
    struct Actions
    {
        bool UP;
        bool LEFT;
        bool DOWN;
        bool RIGHT;
        bool SHOOT;
    };

    struct KeyStatusMap
    {
        // keyboard section 1
        Actions P1;
        Actions P2;

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
    bool GetApplyAction();
    bool GetCancelAction();

private:
    KeyStatusMap m_keyStatusMap;

    void ProcessInput();
    static mt::v2f DirectionVec(const Actions& d);
};

#endif