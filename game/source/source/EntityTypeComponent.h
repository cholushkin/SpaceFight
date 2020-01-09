#ifndef _ENTITY_TYPE_COMPONENT_H
#define _ENTITY_TYPE_COMPONENT_H

struct EntityTypeComponent final
{
    enum EntityType
    {
        Ship,
        PlasmaBullet,
        Asteroid,
        EnergyStation,
        Planet,
        BlackHole
    };
    EntityTypeComponent(EntityType entityType);
    EntityType m_entityType;
};

#endif