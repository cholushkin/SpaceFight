#include "PlayerTag.h"
#include "InputSystem.h"

#include "windows.h"

using namespace mt;

void InputSystem::Update(float /*dt*/, entt::DefaultRegistry& /*registry*/)
{
    ProcessInput();
}


void InputSystem::ProcessInput()
{
    // keyboard section 1
    m_keyStatusMap.P1.UP = GetKeyState(0x57) < 0; // W key
    m_keyStatusMap.P1.LEFT = GetKeyState(0x41) < 0; // A key
    m_keyStatusMap.P1.DOWN = GetKeyState(0x53) < 0; // S key
    m_keyStatusMap.P1.RIGHT = GetKeyState(0x44) < 0; // D key
    m_keyStatusMap.P1.SHOOT = GetKeyState(VK_SPACE) < 0; // SPACE key

    // keyboard section 2
    m_keyStatusMap.P2.UP = GetKeyState(0x50) < 0; // P key
    m_keyStatusMap.P2.LEFT = GetKeyState(0x4C) < 0; // L key
    m_keyStatusMap.P2.DOWN = GetKeyState(VK_OEM_1) < 0; // ; key
    m_keyStatusMap.P2.RIGHT = GetKeyState(VK_OEM_7) < 0; // ' key
    m_keyStatusMap.P2.SHOOT = GetKeyState(VK_RSHIFT) < 0; // RIGHT SHIFT key

    // keyboard system section
    m_keyStatusMap.Keyboard_Apply = GetKeyState(VK_RETURN) < 0; // ENTER key
    m_keyStatusMap.Keyboard_Cancel = GetKeyState(VK_ESCAPE) < 0; // ESCAPE key
}

v2f InputSystem::DirectionVec(const Directions& d) {
    v2f direction;
    if (d.LEFT)
        direction += v2f(-1.0f, 0.0f);
    if (d.RIGHT)
        direction += v2f(1.0f, 0.0f);
    if (d.UP)
        direction += v2f(0.0f, -1.0f);
    if (d.DOWN)
        direction += v2f(0.0f, 1.0f);
    return direction;
}

v2f InputSystem::GetVectorFromInput(int playerID)
{
    if (playerID == 0)
    {
        return DirectionVec(m_keyStatusMap.P1);
    }
    if (playerID == 1)
    {
        return DirectionVec(m_keyStatusMap.P2);
    }
    return v2f();
}

bool InputSystem::GetShootButton(int /*playerID*/)
{
    return true;
}