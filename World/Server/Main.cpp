#include <Assert.h>
#include <Log.h>
#include <World.h>

int main()
{
    Engine::InstallTerminateHandler();

    Engine::WorldConfig config;
    int tick = Engine::TickWorld(0);

    LOG_INFO("Server: world advanced to tick {} (maxPlayers={}, tickRate={})", tick, config.maxPlayers, config.tickRate);

    return 0;
}
