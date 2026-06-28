#include <Clock.h>
#include <TestFramework.h>

#include <chrono>
#include <thread>

// Clock wraps the monotonic steady_clock, so its timing cannot be asserted to an
// exact value without flakiness. These tests check the invariants that must hold
// regardless of scheduling: elapsed time is non-negative, non-decreasing, advances
// across a sleep (lower bound only), and Restart resets the mark.

namespace
{
    void SleepFor(int milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
} // namespace

TEST(Clock, StartsNearZero)
{
    const Engine::Clock clock;
    const Engine::Seconds elapsed = clock.GetElapsed();

    EXPECT_TRUE(elapsed.count() >= 0.0);
    EXPECT_TRUE(elapsed.count() < 1.0);
}

TEST(Clock, ElapsedIsMonotonicNonDecreasing)
{
    const Engine::Clock clock;
    const Engine::Seconds first = clock.GetElapsed();
    SleepFor(5);
    const Engine::Seconds second = clock.GetElapsed();

    EXPECT_TRUE(second.count() >= first.count());
}

TEST(Clock, ElapsedAdvancesAfterSleep)
{
    const Engine::Clock clock;
    SleepFor(20);

    // Lower bound only: the sleep is at least ~20ms, so allow generous slack.
    EXPECT_TRUE(clock.GetElapsed().count() >= 0.010);
}

TEST(Clock, RestartReturnsElapsedAndResets)
{
    Engine::Clock clock;
    SleepFor(20);

    const Engine::Seconds reported = clock.Restart();
    EXPECT_TRUE(reported.count() >= 0.010);

    // Immediately after Restart the mark is fresh, so elapsed is below what the
    // previous interval reported.
    EXPECT_TRUE(clock.GetElapsed().count() < reported.count());
}
