#include <Clock.h>

#include <chrono>

namespace Engine
{
    Clock::Clock()
        : start_(SteadyClock::now())
    {
    }

    Seconds Clock::GetElapsed() const
    {
        return std::chrono::duration_cast<Seconds>(SteadyClock::now() - start_);
    }

    Seconds Clock::Restart()
    {
        const TimePoint now = SteadyClock::now();
        const Seconds elapsed = std::chrono::duration_cast<Seconds>(now - start_);
        start_ = now;
        return elapsed;
    }
} // namespace Engine
