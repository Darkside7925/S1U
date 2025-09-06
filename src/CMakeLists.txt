set(S1U_SOURCES
    main.cpp
    display_server.cpp
    window_manager.cpp
    renderer.cpp
    input_manager.cpp
    compositor.cpp
)

add_executable(s1u ${S1U_SOURCES})

target_link_libraries(s1u
    Threads::Threads
    OpenGL::GL
    GLEW::GLEW
    glfw
    sfml-graphics
    sfml-window
    sfml-system
    ${X11_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

if(MSVC)
    target_compile_options(s1u PRIVATE /O2 /DNDEBUG)
else()
    target_compile_options(s1u PRIVATE -O3 -march=native -mtune=native)
endif()
