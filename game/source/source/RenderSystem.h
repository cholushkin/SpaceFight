#ifndef _RENDER_SYSTEM_H_
#define _RENDER_SYSTEM_H_

#include "core/render/r_render.h"
#include "entt/entity/registry.hpp"

struct GameResources;

// simple hardcoded render
class RenderSystem
{
public:
    RenderSystem();
    void Update(float dt, entt::DefaultRegistry& registry);
    void Render(r::Render& r, GameResources& gRes, entt::DefaultRegistry& registry);

private:
    u32 m_planetSprites[6];
    float m_planetRotations[6];
    float m_planetRotationSpeed[6];
    u32 m_bgSprites[4];
    float m_bgAlphas[4];
    float m_bgAngles[4];
    mt::v2f m_bgPositions[4]; // initially it is treated as a range of radius but later as an abs pos
};

#endif