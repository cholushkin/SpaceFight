#ifndef _GAME_RULES_SYSTEM_H_
#define _GAME_RULES_SYSTEM_H_

#include "entt/entity/registry.hpp"
#include "Level.h"

class GameRulesSystem final
{
public:
    GameRulesSystem(Level& level);
    ~GameRulesSystem() = default;
    void Update(float dt, entt::DefaultRegistry& registry);
    bool HasWinner() const;
    bool GetWinnerID() const;
private:
    int m_winnerID;
    Level& m_level;
};

#endif