#ifndef _PLAYER_COMPONENT_H
#define _PLAYER_COMPONENT_H

struct PlayerComponent final
{
    PlayerComponent(int playerID);

    int m_playerID;
    int m_winCount;
    float m_energy;
    float m_weaponStatus;
};

#endif