add_library(Engine STATIC
    Private/Assert.cpp
    Private/Clock.cpp
    Private/Error.cpp
    Private/Log.cpp
    Private/World.cpp)

target_include_directories(Engine
    PUBLIC Public
    PRIVATE Private)

target_compile_features(Engine PUBLIC cxx_std_20)

ApplyCompilerWarnings(Engine)
