#include "EnergyStationComponent.h"
#include "config.h"

EnergyStationComponent::EnergyStationComponent(float energy)
    : m_energy(energy)
{    
}

bool EnergyStationComponent::HasEnergy() const
{ 
    return m_energy >= ENERGY_STATION_CAPACITY * 0.2f; // if station has 20% of a battery it start to charge clients
}
 