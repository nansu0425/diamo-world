add_library(TestingFramework STATIC
    Private/TestFramework.cpp
    Private/TestMain.cpp)

target_include_directories(TestingFramework
    PUBLIC Public
    PRIVATE Private)

target_compile_features(TestingFramework PUBLIC cxx_std_20)

ApplyCompilerWarnings(TestingFramework)
