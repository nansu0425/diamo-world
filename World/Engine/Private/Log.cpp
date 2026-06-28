#include <Log.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <string_view>

namespace
{
    // Function-local statics (Meyers) avoid static init/destruction order issues
    // if a log fires during startup or shutdown.
    std::atomic<Engine::LogLevel>& GetLogLevelStorage()
    {
        static std::atomic<Engine::LogLevel> level{Engine::LogLevel::Info};
        return level;
    }

    std::mutex& GetOutputMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    // Fixed-width tags so the level column aligns. Indexed by LogLevel value;
    // the Off sentinel is a threshold only and never reaches this table.
    constexpr std::array<std::string_view, 6> LevelTags = {
        "TRACE  ",
        "DEBUG  ",
        "INFO   ",
        "WARNING",
        "ERROR  ",
        "FATAL  ",
    };

    std::string_view GetLevelTag(Engine::LogLevel level) noexcept
    {
        return LevelTags[static_cast<std::size_t>(level)];
    }
} // namespace

namespace Engine
{
    void SetLogLevel(LogLevel level)
    {
        GetLogLevelStorage().store(level, std::memory_order_relaxed);
    }

    LogLevel GetLogLevel() noexcept
    {
        return GetLogLevelStorage().load(std::memory_order_relaxed);
    }

    bool IsLogLevelEnabled(LogLevel level) noexcept
    {
        return level >= GetLogLevel();
    }

    void LogMessage(LogLevel level, std::source_location location, std::string_view message)
    {
        const auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());

        std::string_view file = location.file_name();
        const std::size_t separator = file.find_last_of("/\\");
        if (separator != std::string_view::npos)
        {
            file = file.substr(separator + 1);
        }

        const std::string line = std::format("[{:%H:%M:%S}] [{}] {} ({}:{})", now, GetLevelTag(level), message, file, location.line());

        // Warning and above are diagnostics: route to stderr (unit-buffered, so
        // not lost on a crash) and keep informational output on stdout.
        std::ostream& stream = (level >= LogLevel::Warning) ? std::cerr : std::cout;

        const std::lock_guard<std::mutex> guard(GetOutputMutex());
        stream << line << '\n';
    }
} // namespace Engine
