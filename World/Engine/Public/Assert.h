#pragma once

#include <source_location>
#include <string_view>

namespace Engine
{
    // Writes a Fatal line for a failed assertion to stderr, then aborts; never
    // returns. Self-contained — it depends on no other engine system, so any
    // system (including Log) may use CHECK/ASSERT without a cycle. The reported
    // file:line comes from the GIVEN location (the assertion's call site, not this
    // function's). Defined in Assert.cpp. expression is the stringized condition;
    // message may be empty.
    [[noreturn]] void ReportAssertionFailure(std::source_location location, std::string_view expression, std::string_view message);

    // Installs a std::terminate handler that writes a Fatal line (consistent with
    // an assertion failure) for an exception that escaped to std::terminate — e.g.
    // a std::bad_alloc from an exhausted allocation — then aborts. Call once at
    // program startup. Defined in Assert.cpp.
    void InstallTerminateHandler();
} // namespace Engine

// ASSERT_* compile in only when this is 1. Default it from the build's debug
// signal (_DEBUG, set by the debug CRT), but leave it overridable on the command
// line so an asserts-on diagnostic build is possible regardless of optimization.
// Mirrors Log.h's ENGINE_LOG_MINIMUM_LEVEL: one policy switch, in one place.
#ifndef ENGINE_ENABLE_ASSERTS
#ifdef _DEBUG
#define ENGINE_ENABLE_ASSERTS 1
#else
#define ENGINE_ENABLE_ASSERTS 0
#endif
#endif

// Shared core: evaluate the condition; on failure capture THIS call site and
// report. ::std::source_location::current() expands here, so it names the
// assertion's file:line, exactly like ENGINE_LOG_IMPL.
#define ENGINE_ASSERT_IMPL(condition, message)                                                          \
    do                                                                                                  \
    {                                                                                                   \
        if (!(condition))                                                                               \
        {                                                                                               \
            ::Engine::ReportAssertionFailure(::std::source_location::current(), #condition, (message)); \
        }                                                                                               \
    } while (false)

// CHECK_* — always compiled in, every build config. Guards invariants that must
// hold even in release.
#define CHECK(condition) ENGINE_ASSERT_IMPL((condition), ::std::string_view{})
#define CHECK_MESSAGE(condition, message) ENGINE_ASSERT_IMPL((condition), ::std::string_view{(message)})

// ASSERT_* — debug-only. When elided they expand to ((void)0), which names
// neither argument, so /W4 /WX raises no unused warning. (A symbol used only
// inside an ASSERT will go unused in release; that is inherent to a debug-only
// assert and is the call site's responsibility — use CHECK or mark it.)
#if ENGINE_ENABLE_ASSERTS
#define ASSERT(condition) ENGINE_ASSERT_IMPL((condition), ::std::string_view{})
#define ASSERT_MESSAGE(condition, message) ENGINE_ASSERT_IMPL((condition), ::std::string_view{(message)})
#else
#define ASSERT(condition) ((void)0)
#define ASSERT_MESSAGE(condition, message) ((void)0)
#endif
