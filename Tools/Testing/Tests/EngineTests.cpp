#include <TestFramework.h>
#include <World.h>

TEST(World, Tick)
{
    EXPECT_EQUAL(Engine::TickWorld(0), 1);
    EXPECT_EQUAL(Engine::TickWorld(41), 42);
}

TEST(World, Config)
{
    Engine::WorldConfig config;
    EXPECT_TRUE(config.maxPlayers > 0);
    EXPECT_TRUE(config.tickRate > 0);
}
