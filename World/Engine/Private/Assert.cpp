#include <Assert.h>

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace
{
    constexpr std::string_view BannerTop = "======== ENGINE ABORT ========";
    constexpr std::string_view BannerBottom = "==============================";

    // Writes the abort banner to stderr; empty fields are omitted. The multi-line
    // banner is deliberately unlike Log's single line, so an abort is obvious at a
    // glance and stays distinct even in a file where color is stripped. The error
    // system is self-contained: the fatal path never routes through Log — no
    // dependency cycle, no re-entrant call back into a failing sink.
    void WriteAbortBanner(std::string_view reason, std::string_view check, std::string_view message, std::string_view location)
    {
        const auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());

        std::string banner = std::format("{}\n time     : {:%H:%M:%S}\n reason   : {}\n", BannerTop, now, reason);
        if (!check.empty())
        {
            banner += std::format(" check    : {}\n", check);
        }
        if (!message.empty())
        {
            banner += std::format(" message  : {}\n", message);
        }
        if (!location.empty())
        {
            banner += std::format(" at       : {}\n", location);
        }
        banner += BannerBottom;

        std::cerr << banner << '\n';
        std::cerr.flush();
    }

    // Trims a path to its basename so a location matches the LOG_* style.
    std::string_view BaseName(std::string_view path)
    {
        const std::size_t separator = path.find_last_of("/\\");
        return separator == std::string_view::npos ? path : path.substr(separator + 1);
    }

    // The installed std::terminate handler: report and abort. std::terminate is
    // reached when an exception escapes (e.g. a std::bad_alloc from an exhausted
    // allocation) or terminate is called directly; either way the process cannot
    // continue. We do not catch to inspect the exception: World is catch-free, and
    // clang-tidy parses it with exceptions disabled, so a catch would not build.
    [[noreturn]] void HandleTerminate() noexcept
    {
        WriteAbortBanner("Unhandled exception or terminate called", {}, {}, {});
        std::abort();
    }
} // namespace

namespace Engine
{
    void ReportAssertionFailure(std::source_location location, std::string_view expression, std::string_view message)
    {
        const std::string at = std::format("{}:{}", BaseName(location.file_name()), location.line());
        WriteAbortBanner("Assertion failed", expression, message, at);

        // The invariant is broken; continuing would run on corrupt state. abort
        // takes the platform crash path (debugger break / Windows Error Reporting).
        std::abort();
    }

    void InstallTerminateHandler()
    {
        std::set_terminate(&HandleTerminate);
    }
} // namespace Engine
