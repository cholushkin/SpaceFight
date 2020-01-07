#include "PlayerTag.h"
#include "InputSystem.h"

#include "windows.h"

void InputSystem::Update(float /*dt*/, entt::DefaultRegistry& /*registry*/)
{
    ProcessInput();
}


void InputSystem::ProcessInput()
{
    // keyboard section 1
    m_keyStatusMap.KeyboardP1_UP = GetKeyState(0x57) < 0; // W key
    m_keyStatusMap.KeyboardP1_LEFT = GetKeyState(0x41) < 0; // A key
    m_keyStatusMap.KeyboardP1_DOWN = GetKeyState(0x53) < 0; // S key
    m_keyStatusMap.KeyboardP1_RIGHT = GetKeyState(0x44) < 0; // D key
    m_keyStatusMap.KeyboardP1_SHOOT = GetKeyState(VK_SPACE) < 0; // SPACE key

    // keyboard section 2
    m_keyStatusMap.KeyboardP2_UP = GetKeyState(0x50) < 0; // P key
    m_keyStatusMap.KeyboardP2_LEFT = GetKeyState(0x4C) < 0; // L key
    m_keyStatusMap.KeyboardP2_DOWN = GetKeyState(VK_OEM_1) < 0; // ; key
    m_keyStatusMap.KeyboardP2_RIGHT = GetKeyState(VK_OEM_7) < 0; // ' key
    m_keyStatusMap.KeyboardP2_SHOOT = GetKeyState(VK_RSHIFT) < 0; // RIGHT SHIFT key

    // keyboard system section
    m_keyStatusMap.Keyboard_Apply = GetKeyState(VK_RETURN) < 0; // ENTER key
    m_keyStatusMap.Keyboard_Cancel = GetKeyState(VK_ESCAPE) < 0; // ESCAPE key
}