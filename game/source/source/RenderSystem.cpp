#include "RenderSystem.h"
#include "PhysicsAgentComponent.h"
#include "ext/draw2d/draw_helper.h"
#include "ext/math/mt_colors.h"
#include "config.h"

using namespace mt;
using namespace r;

void RenderSystem::Update(float /*dt*/, entt::DefaultRegistry& /*registry*/)
{

}

void RenderSystem::Render(r::Render& r, entt::DefaultRegistry& registry)
{
#ifdef _DEBUG
    registry.view<PhysicsAgentComponent>().each([&](auto /*entity*/, PhysicsAgentComponent& psxComp)
    {
        DrawHelper dr(r);
        
        static const v2f offset = v2f(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
        dr.SetTransform(offset);

        dr.DrawCircle(v2f(), 10.0f, 16, COLOR_YELLOW);
        dr.DrawCircle(psxComp.m_agent->pos, psxComp.m_agent->radius, 16, COLOR_WHITE);
        

        //if (sc.m_radius != 0)
        //{
        //    // We use SDL2_gfx to make drawing circles easier.
        //    filledCircleRGBA(window->getRenderer(), static_cast<Sint16>(pc.m_x), static_cast<Sint16>(pc.m_y), sc.m_radius, sc.m_colour.r, sc.m_colour.g, sc.m_colour.b, sc.m_colour.a);
        //}
        //else
        //{
        //    // First we set the rectangle fill colour to that of the spritecomponents.
        //    SDL_SetRenderDrawColor(window->getRenderer(), sc.m_colour.r, sc.m_colour.g, sc.m_colour.b, sc.m_colour.a);

        //    // Then we create the actual rectangle.
        //    SDL_Rect rectToDraw{ static_cast<int>(pc.m_x), static_cast<int>(pc.m_y), sc.m_width, sc.m_height };

        //    // Now the rectangle gets renderered with the appropriate colours and position data to the window.
        //    SDL_RenderFillRect(window->getRenderer(), &rectToDraw);
        //}
    });
#endif
}

