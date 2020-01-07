#ifndef _PLAYERTAG_HPP_
#define _PLAYERTAG_HPP_

// Used to retrieve player entity.
class PlayerTag final
{
public:
    PlayerTag() = default;
    ~PlayerTag() = default;
    PlayerTag& operator=(const PlayerTag &) noexcept;
};

#endif