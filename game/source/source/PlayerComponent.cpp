#include "PlayerComponent.h"



PlayerComponent::PlayerComponent(int playerID, int winCount)
    : m_playerID(playerID)
    , m_winCount(winCount)
    , m_energy(100.0f)
    , m_weaponStatus(1.0f)
{    
}