#ifndef _PLAYER_COMPONENT_H
#define _PLAYER_COMPONENT_H

#include "config.h"

struct PlayerComponent final
{
    PlayerComponent(int playerID, int winCount);

    int m_playerID;
    int m_winCount;
    float m_energy;
    float m_weaponStatus;

    void EnergyDamage(float hitpoints)
    {
        m_energy -= hitpoints;
    }

    bool EnergySpend(float hitpoints)
    {
        if (m_energy - hitpoints >= 0.0f)
        {
            m_energy -= hitpoints;
            return true;
        }
        return false;
    }

    void EnergyRecieve(float points)
    {
        m_energy += points;
        if (m_energy >= SHIP_ENERGY_MAX)
            m_energy = SHIP_ENERGY_MAX;
    }
};

#endif