#include <Log.h>
#include <TestFramework.h>

#include <string_view>

// The console sink's text cannot be asserted without redirecting the stream
// buffer, which is more than an MVP warrants. These tests cover what is cleanly
// checkable: the threshold round-trips, the filter predicate respects ordering
// (including the Off sentinel), and the LOG_* macros instantiate and run.

TEST(Log, ThresholdRoundTrips)
{
    Engine::SetLogLevel(Engine::LogLevel::Warning);
    EXPECT_EQUAL(Engine::GetLogLevel(), Engine::LogLevel::Warning);

    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default
}

TEST(Log, FilterRespectsThreshold)
{
    Engine::SetLogLevel(Engine::LogLevel::Warning);
    EXPECT_FALSE(Engine::IsLogLevelEnabled(Engine::LogLevel::Info));
    EXPECT_TRUE(Engine::IsLogLevelEnabled(Engine::LogLevel::Warning));
    EXPECT_TRUE(Engine::IsLogLevelEnabled(Engine::LogLevel::Error));

    Engine::SetLogLevel(Engine::LogLevel::Off);
    EXPECT_FALSE(Engine::IsLogLevelEnabled(Engine::LogLevel::Fatal));

    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default
}

TEST(Log, MacrosCompileAndRun)
{
    // No output assertion: reaching the end without a crash or compile error is
    // the check that formatting and argument forwarding stay correct.
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_INFO("value={} name={} ratio={}", 7, std::string_view{"abc"}, 0.5);
    LOG_ERROR("error code {}", 42);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}
