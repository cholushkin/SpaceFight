#ifndef _ENERGY_RESOURCE_COMPONENT_H
#define _ENERGY_RESOURCE_COMPONENT_H

#include "config.h"

struct EnergyResourceComponent final
{
    EnergyResourceComponent();
    float m_retriveProgression[MAX_PLAYERS];
};

#endif