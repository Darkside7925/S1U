set(DRIVER_SOURCES
    drm_driver.cpp
    nvidia_driver.cpp
    vulkan_driver.cpp
)

add_library(s1u_drivers STATIC ${DRIVER_SOURCES})

target_link_libraries(s1u_drivers
    Threads::Threads
)

target_compile_options(s1u_drivers PRIVATE
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
