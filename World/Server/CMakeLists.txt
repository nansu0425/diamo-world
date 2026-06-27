add_executable(Server
    Main.cpp)

target_link_libraries(Server PRIVATE Engine)

ApplyCompilerWarnings(Server)
