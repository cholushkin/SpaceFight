#ifndef _RENDER_SYSTEM_H_
#define _RENDER_SYSTEM_H_

#include "core/render/r_render.h"
#include "entt/entity/registry.hpp"

struct GameResources;
class RenderSystem
{
public:
    RenderSystem();
    void Update(float dt, entt::DefaultRegistry& registry);
    void Render(r::Render& r, GameResources& gRes, entt::DefaultRegistry& registry);
};

#endif