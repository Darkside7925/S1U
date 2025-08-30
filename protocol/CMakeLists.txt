set(PROTOCOL_SOURCES
    quantum_protocol.cpp
)

add_library(s1u_protocol STATIC ${PROTOCOL_SOURCES})

target_link_libraries(s1u_protocol
    Threads::Threads
)

target_compile_options(s1u_protocol PRIVATE
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
