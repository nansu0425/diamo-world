#include <World.h>

#include <iostream>

int main()
{
    Engine::WorldConfig config;
    int tick = Engine::TickWorld(0);

    std::cout << "Server: world advanced to tick " << tick << " (maxPlayers=" << config.maxPlayers << ", tickRate=" << config.tickRate << ")\n";

    return 0;
}
