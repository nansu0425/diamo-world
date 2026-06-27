#pragma once

namespace Engine
{
    // World runtime parameters.
    struct WorldConfig
    {
        int maxPlayers = 100;
        int tickRate = 60;
    };

    // Advances the world by one tick and returns the next tick number.
    int TickWorld(int currentTick);
} // namespace Engine
