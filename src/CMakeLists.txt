set(S1U_SOURCES
    main.cpp
)

add_executable(s1u ${S1U_SOURCES})

target_link_libraries(s1u
    Threads::Threads
)

if(MSVC)
    target_compile_options(s1u PRIVATE /O2 /DNDEBUG)
else()
    target_compile_options(s1u PRIVATE -O3 -march=native -mtune=native)
endif()
