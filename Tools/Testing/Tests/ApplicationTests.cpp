#include <Application.h>
#include <Shutdown.h>
#include <TestFramework.h>
#include <World.h>

#include <chrono>
#include <thread>

// The application loop runs until stopped, so it is driven here through two seams:
// the bounded variant (deterministic, exact tick counts) and a real stop request
// from another thread (liveness of the cooperative-shutdown path).
//
// IMPORTANT: the stop flag is process-global and has no reset, so once
// StopFromAnotherThreadEndsLoop sets it the loop would exit immediately ever after.
// Keep that test last and keep the bounded tests (which never touch the flag) before
// it. A high tickRate keeps each test's wall-clock time small.

TEST(Application, BoundedRunExecutesExactTickCount)
{
    Engine::WorldConfig config;
    config.tickRate = 1000;

    const Engine::RunReport report = Engine::RunApplicationBounded(config, 10);
    EXPECT_EQUAL(report.ticksExecuted, 10LL);
}

TEST(Application, FinalTickMatchesTicksExecuted)
{
    Engine::WorldConfig config;
    config.tickRate = 1000;

    // TickWorld advances by one from 0, so after N ticks the final tick number is N.
    const Engine::RunReport report = Engine::RunApplicationBounded(config, 10);
    EXPECT_EQUAL(report.finalTick, 10);
}

TEST(Application, StopFromAnotherThreadEndsLoop)
{
    Engine::WorldConfig config;
    config.tickRate = 1000;

    std::thread stopper([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        Engine::RequestStop();
    });

    // No tick limit: the loop returns only because the stop request is observed.
    const Engine::RunReport report = Engine::RunApplication(config);
    stopper.join();

    EXPECT_TRUE(report.ticksExecuted > 0);
}
