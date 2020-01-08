#ifndef _PLANET_COMPONENT_H
#define _PLANET_COMPONENT_H

struct PlanetComponent final
{
public:
    PlanetComponent(int planetType,float scale,float mass,float radius);

public:
    int m_planetType;
    float m_scale; // affects mass and radius
    float m_mass;
    float m_radius;
};

#endif