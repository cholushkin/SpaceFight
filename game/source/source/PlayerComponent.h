#ifndef _PLAYER_COMPONENT_H
#define _PLAYER_COMPONENT_H

struct PlayerComponent final
{
public:
    PlayerComponent(int playerID);

public:
    int m_playerID;
    int m_winCount;
    float m_energy;
};

#endif