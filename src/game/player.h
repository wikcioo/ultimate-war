#pragma once

#include <string>

class Player
{
public:
    Player(int id);
    ~Player() = default;
    inline int GetId() { return m_ID; }
    inline int GetGold() { return m_Gold; }
    inline void AddGold(int amount) { m_Gold += amount; }

private:
    int m_ID;
    int m_Gold;
};
