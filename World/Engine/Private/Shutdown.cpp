#include <Shutdown.h>

#include <Assert.h>

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace
{
    // Function-local static (Meyers) avoids static init/destruction order issues if
    // a stop is requested very early or very late. Relaxed ordering is enough: the
    // flag is a one-way latch and the loop polls it each iteration.
    std::atomic<bool>& GetStopFlag()
    {
        static std::atomic<bool> stopRequested{false};
        return stopRequested;
    }

    // Console control handler. Returns TRUE to mark the event handled (so the OS does
    // not run the default action, which would kill the process) for the events that
    // mean "the user wants this to end"; FALSE lets unrelated events pass through.
    // For close/logoff/shutdown the OS grants only a short window before it terminates
    // the process anyway, but the loop polls the flag every tick and so reacts at once.
    BOOL WINAPI HandleConsoleControl(DWORD controlType)
    {
        switch (controlType)
        {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            Engine::RequestStop();
            return TRUE;
        default:
            return FALSE;
        }
    }
} // namespace

namespace Engine
{
    void InstallShutdownHandler()
    {
        const BOOL installed = SetConsoleCtrlHandler(&HandleConsoleControl, TRUE);
        CHECK_MESSAGE(installed != 0, "Failed to install the console control handler");
    }

    void RequestStop()
    {
        GetStopFlag().store(true, std::memory_order_relaxed);
    }

    bool IsStopRequested()
    {
        return GetStopFlag().load(std::memory_order_relaxed);
    }
} // namespace Engine
