#pragma once

#include <chrono>

namespace Engine
{
    // Monotonic time base. steady_clock is the standard library's monotonic clock
    // (QPC-backed on MSVC); unlike system_clock it never jumps, so it is the right
    // source for measuring elapsed time and loop deltas. system_clock stays where
    // a human-readable wall-clock timestamp is wanted (e.g. log line times).
    using SteadyClock = std::chrono::steady_clock;
    using TimePoint = SteadyClock::time_point;

    // Elapsed time as floating-point seconds — the natural unit for delta and
    // fixed-timestep math, and convertible to and from any std::chrono duration.
    using Seconds = std::chrono::duration<double>;

    // A restartable stopwatch over the monotonic clock. Construction marks the
    // start; GetElapsed reads without resetting, Restart reads and re-marks.
    class Clock
    {
    public:
        Clock();

        // Time since the start mark, without moving it.
        Seconds GetElapsed() const;

        // Returns the time since the start mark, then moves the mark to now.
        Seconds Restart();

    private:
        TimePoint start_;
    };
} // namespace Engine
