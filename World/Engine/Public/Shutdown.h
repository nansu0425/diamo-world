#pragma once

namespace Engine
{
    // Installs a console control handler (Ctrl+C, Ctrl+Break, console close, logoff,
    // and system shutdown) that requests a cooperative stop instead of letting the OS
    // terminate the process. Call once at startup, like InstallTerminateHandler. Uses
    // the platform console API directly (the standard library has no equivalent).
    // Defined in Shutdown.cpp.
    void InstallShutdownHandler();

    // Requests a cooperative stop. The application loop observes it and returns. Safe
    // to call from any thread, including a console control handler thread.
    void RequestStop();

    // Whether a stop has been requested. Cheap and lock-free.
    bool IsStopRequested();
} // namespace Engine
