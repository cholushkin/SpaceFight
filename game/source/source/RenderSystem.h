#ifndef _RENDER_SYSTEM_H_
#define _RENDER_SYSTEM_H_

#include "core/render/r_render.h"
#include "entt/entity/registry.hpp"

class RenderSystem
{
public:
    //RenderSystem() = default;
    //~RenderSystem() = default;
    //RenderSystem& operator=(const RenderSystem &) noexcept;
    void Update(float dt, entt::DefaultRegistry& registry);
    void Render(r::Render& r, entt::DefaultRegistry& registry);
};

#endif