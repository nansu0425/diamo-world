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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

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

    // Level tags at natural width. Indexed by LogLevel value; the Off sentinel is
    // a threshold only and never reaches this table.
    constexpr std::array<std::string_view, 6> LevelTags = {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    std::string_view GetLevelTag(Engine::LogLevel level) noexcept
    {
        return LevelTags[static_cast<std::size_t>(level)];
    }

    // ANSI SGR foreground colors per level, indexed like LevelTags. The level tag
    // carries the severity signal, so it gets the color; the message body stays
    // default. Fatal is bold red to stand apart from a plain Error.
    constexpr std::array<std::string_view, 6> LevelColors = {
        "\x1b[90m",   // Trace   - bright black (gray)
        "\x1b[36m",   // Debug   - cyan
        "\x1b[32m",   // Info    - green
        "\x1b[33m",   // Warning - yellow
        "\x1b[31m",   // Error   - red
        "\x1b[1;31m", // Fatal   - bold red
    };

    std::string_view GetLevelColor(Engine::LogLevel level) noexcept
    {
        return LevelColors[static_cast<std::size_t>(level)];
    }

    // Turns on ANSI escape handling for one standard stream. GetConsoleMode only
    // succeeds for a real console, so a stream redirected to a file or pipe fails
    // here and is treated as non-color (keeping escape codes out of log files).
    bool EnableVirtualTerminal(DWORD stdHandleId) noexcept
    {
        const HANDLE handle = GetStdHandle(stdHandleId);
        if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        DWORD mode = 0;
        if (!GetConsoleMode(handle, &mode))
        {
            return false;
        }
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        {
            return true;
        }
        return SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }

    struct ColorCapability
    {
        bool stdoutColor;
        bool stderrColor;
    };

    // Enables virtual terminal processing once (magic static => thread-safe) and
    // caches whether each stream can render color. stdout and stderr are separate
    // handles because Warn and above route to stderr.
    const ColorCapability& GetColorCapability()
    {
        static const ColorCapability capability{
            EnableVirtualTerminal(STD_OUTPUT_HANDLE),
            EnableVirtualTerminal(STD_ERROR_HANDLE),
        };
        return capability;
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

        // Warn and above are diagnostics: route to stderr (unit-buffered, so
        // not lost on a crash) and keep informational output on stdout.
        const bool toStderr = (level >= LogLevel::Warn);
        std::ostream& stream = toStderr ? std::cerr : std::cout;

        const ColorCapability& capability = GetColorCapability();
        const bool useColor = toStderr ? capability.stderrColor : capability.stdoutColor;

        std::string line;
        if (useColor)
        {
            // Color the level tag by severity; dim the timestamp and call site so
            // the default-color message body stands out. Each region is reset.
            constexpr std::string_view Dim = "\x1b[2m";
            constexpr std::string_view Reset = "\x1b[0m";
            line = std::format("{0}[{1:%H:%M:%S}]{2} [{3}{4}{2}] {5} {0}({6}:{7}){2}", Dim, now, Reset,
                               GetLevelColor(level), GetLevelTag(level), message, file, location.line());
        }
        else
        {
            line = std::format("[{:%H:%M:%S}] [{}] {} ({}:{})", now, GetLevelTag(level), message, file, location.line());
        }

        const std::lock_guard<std::mutex> guard(GetOutputMutex());
        stream << line << '\n';
    }
} // namespace Engine
