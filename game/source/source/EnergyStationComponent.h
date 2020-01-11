#ifndef _ENERGY_STATION_COMPONENT_H
#define _ENERGY_STATION_COMPONENT_H

struct EnergyStationComponent final
{
    EnergyStationComponent(float energy);
    float m_energy; // if > 0 then it is in process of generating next one
    bool HasEnergy() const;
};

#endif