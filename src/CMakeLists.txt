set(S1U_SOURCES
    core.cpp
    window.cpp
    display.cpp
    compositor.cpp
    renderer.cpp
    input_manager.cpp
    driver_manager.cpp
    protocol_server.cpp
    main.cpp
)

add_executable(s1u ${S1U_SOURCES})

target_link_libraries(s1u
    ${DRM_LIBRARIES}
    ${GBM_LIBRARIES}
    ${EGL_LIBRARIES}
    ${GLES2_LIBRARIES}
    ${INPUT_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${PANGO_LIBRARIES}
    Threads::Threads
    dl
    m
    rt
)

target_compile_options(s1u PRIVATE
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
