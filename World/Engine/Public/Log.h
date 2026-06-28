#pragma once

#include <format>
#include <source_location>
#include <string_view>
#include <utility>

namespace Engine
{
    // Severity levels, ascending. Off is a threshold-only sentinel (suppresses
    // everything); it is never passed to a write.
    enum class LogLevel : int
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Fatal = 5,
        Off = 6,
    };

    // Sets the runtime minimum severity; messages below it are dropped.
    void SetLogLevel(LogLevel level);

    // Returns the current runtime minimum severity.
    LogLevel GetLogLevel() noexcept;

    // Returns whether a message at this level would currently be emitted
    // (level >= threshold). Cheap and lock-free.
    bool IsLogLevelEnabled(LogLevel level) noexcept;

    // Writes one already-formatted line to the console sink. The sole place that
    // touches the sink; future sinks change only this function. Defined in Log.cpp.
    void LogMessage(LogLevel level, std::source_location location, std::string_view message);

    // Formats with std::format and forwards to LogMessage. Prefer the LOG_* macros,
    // which inject the call site and allow compile-time elision. format is taken by
    // value (not forwarded) so std::format_string deduction stays intact.
    template <typename... Args>
    void WriteLog(LogLevel level, std::source_location location, std::format_string<Args...> format, Args&&... args)
    {
        if (!IsLogLevelEnabled(level))
        {
            return;
        }
        LogMessage(level, location, std::format(format, std::forward<Args>(args)...));
    }
} // namespace Engine

// Builds at or above this level are compiled in; lower ones are elided entirely.
// 0 == Trace keeps every call site.
#ifndef ENGINE_LOG_MINIMUM_LEVEL
#define ENGINE_LOG_MINIMUM_LEVEL 0
#endif

// Injects the call-site source location and wraps the call in if constexpr so an
// elided level never odr-uses its arguments (no /WX "unused" warning).
#define ENGINE_LOG_IMPL(level, ...)                                                      \
    do                                                                                   \
    {                                                                                    \
        if constexpr (static_cast<int>(level) >= ENGINE_LOG_MINIMUM_LEVEL)               \
        {                                                                                \
            ::Engine::WriteLog((level), ::std::source_location::current(), __VA_ARGS__); \
        }                                                                                \
    } while (false)

#define LOG_TRACE(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARNING(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) ENGINE_LOG_IMPL(::Engine::LogLevel::Fatal, __VA_ARGS__)
