#include <Application.h>
#include <Assert.h>
#include <Log.h>
#include <Shutdown.h>
#include <World.h>

int main()
{
    Engine::InstallTerminateHandler();
    Engine::InstallShutdownHandler();

    Engine::WorldConfig config;
    LOG_INFO("Client: connecting to world (tickRate={})", config.tickRate);

    const Engine::RunReport report = Engine::RunApplication(config);

    LOG_INFO("Client: disconnected at tick {} after {} ticks", report.finalTick, report.ticksExecuted);
    return 0;
}
