#ifndef _GAME_RULES_SYSTEM_H_
#define _GAME_RULES_SYSTEM_H_

#include "entt/entity/registry.hpp"

class GameRulesSystem final
{
public:
    GameRulesSystem();
    ~GameRulesSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);
};

#endif