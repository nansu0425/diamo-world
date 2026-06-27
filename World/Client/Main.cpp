#include <World.h>

#include <iostream>

int main()
{
    int tick = Engine::TickWorld(0);

    std::cout << "Client: connected to world at tick " << tick << "\n";
    return 0;
}
