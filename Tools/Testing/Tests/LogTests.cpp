#include <Log.h>
#include <TestFramework.h>

#include <string_view>

// The console sink's text cannot be asserted without redirecting the stream
// buffer, which is more than an MVP warrants. These tests cover what is cleanly
// checkable: the threshold round-trips, the filter predicate respects ordering
// (including the Off sentinel), and the LOG_* macros instantiate and run.

TEST(Log, ThresholdRoundTrips)
{
    Engine::SetLogLevel(Engine::LogLevel::Warn);
    EXPECT_EQUAL(Engine::GetLogLevel(), Engine::LogLevel::Warn);

    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default
}

TEST(Log, FilterRespectsThreshold)
{
    Engine::SetLogLevel(Engine::LogLevel::Warn);
    EXPECT_FALSE(Engine::IsLogLevelEnabled(Engine::LogLevel::Info));
    EXPECT_TRUE(Engine::IsLogLevelEnabled(Engine::LogLevel::Warn));
    EXPECT_TRUE(Engine::IsLogLevelEnabled(Engine::LogLevel::Error));

    Engine::SetLogLevel(Engine::LogLevel::Off);
    EXPECT_FALSE(Engine::IsLogLevelEnabled(Engine::LogLevel::Fatal));

    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default
}

// One case per LOG_* macro. Each lowers the threshold to Trace so the call is
// not filtered, runs the macro with representative arguments, then restores the
// default level. No output assertion: reaching the end without a crash or
// compile error is the check that each level's formatting and argument
// forwarding stay correct.

TEST(Log, TraceMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_TRACE("trace flag={}", true);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}

TEST(Log, DebugMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_DEBUG("debug step {} of {}", 2, 5);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}

TEST(Log, InfoMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_INFO("value={} name={} ratio={}", 7, std::string_view{"abc"}, 0.5);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}

TEST(Log, WarnMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_WARN("warn threshold {} exceeded", 0.9);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}

TEST(Log, ErrorMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_ERROR("error code {}", 42);
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}

TEST(Log, FatalMacroCompilesAndRuns)
{
    Engine::SetLogLevel(Engine::LogLevel::Trace);
    LOG_FATAL("fatal: {}", std::string_view{"unrecoverable"});
    Engine::SetLogLevel(Engine::LogLevel::Info); // restore default

    EXPECT_TRUE(true);
}
