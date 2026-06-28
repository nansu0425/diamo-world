#include <Assert.h>
#include <Log.h>
#include <World.h>

int main()
{
    Engine::InstallTerminateHandler();

    int tick = Engine::TickWorld(0);

    LOG_INFO("Client: connected to world at tick {}", tick);
    return 0;
}
