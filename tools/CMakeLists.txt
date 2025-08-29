set(TOOL_SOURCES
    s1u_client_tool.cpp
    s1u_debug_tool.cpp
    s1u_profile_tool.cpp
    s1u_benchmark_tool.cpp
    s1u_test_tool.cpp
    s1u_monitor_tool.cpp
    s1u_config_tool.cpp
    s1u_install_tool.cpp
    s1u_update_tool.cpp
    s1u_clean_tool.cpp
)

add_executable(s1u_client ${TOOL_SOURCES})

target_link_libraries(s1u_client
    s1u_protocol
    Threads::Threads
    dl
    m
    rt
)

target_compile_options(s1u_client PRIVATE
    -O3
    -march=native
    -mtune=native
    -flto
    -fomit-frame-pointer
    -ffast-math
    -funroll-loops
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
    -Werror
)
