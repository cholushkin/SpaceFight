#include "PlayerComponent.h"



PlayerComponent::PlayerComponent(int playerID)
    : m_playerID(playerID)
    , m_winCount(0)
    , m_energy(100.0f)
    , m_weaponStatus(1.0f)
{    
}