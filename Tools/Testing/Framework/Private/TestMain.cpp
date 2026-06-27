#include <TestFramework.h>

// Entry point for test executables. Test sources only write TEST(...);
// linking TestingFramework provides this main(), which runs all tests.
int main()
{
    return ::Testing::RunAllTests();
}
