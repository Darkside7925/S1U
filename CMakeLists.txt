cmake_minimum_required(VERSION 3.16)
project(S1U VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

find_package(Threads REQUIRED)

# Graphics and display libraries
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# GLFW
find_package(glfw3 REQUIRED)

# SFML for multimedia support
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# X11 libraries for Linux
if(UNIX AND NOT APPLE)
    pkg_check_modules(X11 REQUIRED x11 xext xrender xrandr)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)


# GLFW
find_package(glfw3 REQUIRED)

# SFML for multimedia support
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# X11 libraries for Linux
if(UNIX AND NOT APPLE)
    pkg_check_modules(X11 REQUIRED x11 xext xrender xrandr)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)


# GLFW
find_package(glfw3 REQUIRED)

# SFML for multimedia support
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# X11 libraries for Linux
if(UNIX AND NOT APPLE)
    pkg_check_modules(X11 REQUIRED x11 xext xrender xrandr)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
