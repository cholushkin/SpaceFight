#ifndef _ENERGY_STATION_COMPONENT_H
#define _ENERGY_STATION_COMPONENT_H

struct EnergyStationComponent final
{
    EnergyStationComponent(float energy);
    float m_energy; // accumulated energy quantity
    bool HasEnergy() const;
};

#endif