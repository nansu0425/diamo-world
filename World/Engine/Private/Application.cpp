#include <Application.h>

#include <Assert.h>
#include <Clock.h>
#include <Log.h>
#include <Shutdown.h>

#include <chrono>
#include <thread>

namespace
{
    // Cap on simulation ticks run in a single loop iteration. After a stall the
    // accumulator can hold many ticks' worth of time; running them all unbounded
    // would let work fall further behind each iteration (the "spiral of death").
    // Past this cap the remaining backlog is dropped instead.
    constexpr int MaxCatchUpTicks = 5;
} // namespace

namespace Engine
{
    RunReport RunApplicationBounded(const WorldConfig& config, long long maxTicks)
    {
        CHECK_MESSAGE(config.tickRate > 0, "RunApplication requires a positive tickRate");

        // tickInterval is the wall-clock time one tick represents; tickIntervalDuration
        // is the same span in the clock's own units, for advancing the deadline.
        const Seconds tickInterval{1.0 / config.tickRate};
        const auto tickIntervalDuration = std::chrono::duration_cast<SteadyClock::duration>(tickInterval);

        LOG_INFO("application loop start: tickRate={} tickInterval={:.3f}ms", config.tickRate, tickInterval.count() * 1000.0);

        int tick = 0;
        long long ticksExecuted = 0;
        Seconds accumulator{0.0};
        TimePoint previous = SteadyClock::now();
        TimePoint nextDeadline = previous + tickIntervalDuration;

        const auto reachedLimit = [&]() { return maxTicks > 0 && ticksExecuted >= maxTicks; };

        while (!IsStopRequested() && !reachedLimit())
        {
            const TimePoint now = SteadyClock::now();
            accumulator += std::chrono::duration_cast<Seconds>(now - previous);
            previous = now;

            // Number of ticks run this iteration, capped to bound catch-up work.
            int catchUpTicks = 0;
            while (accumulator >= tickInterval && catchUpTicks < MaxCatchUpTicks && !IsStopRequested() && !reachedLimit())
            {
                tick = TickWorld(tick);
                ++ticksExecuted;
                accumulator -= tickInterval;
                ++catchUpTicks;
            }

            // Still behind after the cap: drop the backlog so the loop does not spiral.
            if (accumulator >= tickInterval)
            {
                const long long backlogTicks = static_cast<long long>(accumulator / tickInterval);
                LOG_WARN("application loop behind by {} tick(s); dropping backlog", backlogTicks);
                accumulator = Seconds{0.0};
                nextDeadline = SteadyClock::now();
            }

            nextDeadline += tickIntervalDuration;
            const TimePoint afterWork = SteadyClock::now();
            if (nextDeadline > afterWork)
            {
                std::this_thread::sleep_until(nextDeadline);
            }
            else
            {
                // Late: do not bank the missed slack into the next deadline.
                nextDeadline = afterWork;
            }
        }

        LOG_INFO("application loop stop: ticksExecuted={} finalTick={}", ticksExecuted, tick);
        return RunReport{ticksExecuted, tick};
    }

    RunReport RunApplication(const WorldConfig& config)
    {
        return RunApplicationBounded(config, 0);
    }
} // namespace Engine
