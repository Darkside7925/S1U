set(S1U_SOURCES
    core.cpp
    window.cpp
    main.cpp
    realtime_optimizer.cpp
    extreme_performance_optimizer.cpp
    quantum_memory_manager.cpp
    advanced_display_manager.cpp
    quantum_network_protocol.cpp
    neural_scheduler.cpp
    holographic_display_system.cpp
    quantum_optimization_engine.cpp
    quantum_field_processor.cpp
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
    Vulkan::Vulkan
    Threads::Threads
    dl
    m
    rt
    numa
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
