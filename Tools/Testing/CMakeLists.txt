add_subdirectory(Framework)

# Test executable — links TestingFramework (runner + main) and the unit under test (Engine).
add_executable(EngineTests
    Tests/EngineTests.cpp
    Tests/LogTests.cpp
    Tests/ErrorTests.cpp
    Tests/AssertTests.cpp
    Tests/ClockTests.cpp)

target_link_libraries(EngineTests PRIVATE TestingFramework Engine)

ApplyCompilerWarnings(EngineTests)

# Register with CTest — pass/fail is decided by the executable's exit code (0 = pass).
add_test(NAME EngineTests COMMAND EngineTests)
