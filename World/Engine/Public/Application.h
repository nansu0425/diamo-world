#pragma once

#include <World.h>

namespace Engine
{
    // The outcome of one application run, so a caller or test can observe progress
    // without reaching into globals.
    struct RunReport
    {
        long long ticksExecuted = 0;
        int finalTick = 0;
    };

    // Runs the fixed-timestep loop, advancing TickWorld at config.tickRate, until a
    // stop is requested (see Shutdown.h). Blocks the calling thread. config.tickRate
    // must be positive (checked). Returns what the run did.
    RunReport RunApplication(const WorldConfig& config);

    // Same loop, but also stops after maxTicks ticks (maxTicks <= 0 means no limit).
    // The bound makes the loop deterministically terminable from a test.
    RunReport RunApplicationBounded(const WorldConfig& config, long long maxTicks);
} // namespace Engine
