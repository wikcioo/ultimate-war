#pragma once

#include <memory>

#include "game/player.h"

class AI
{
public:
    AI();
    ~AI() = default;

    static AI& Get() { return *s_Instance; }
    static void MakeMove(const std::shared_ptr<Player>& player);

private:
    static AI* s_Instance;
};
