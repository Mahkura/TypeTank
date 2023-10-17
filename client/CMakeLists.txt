cmake_minimum_required(VERSION 3.0.0)
project(TypeTanks VERSION 1.0.0)

option(TypeTanks_VENDORED "Use vendored libraries" OFF)

if(TypeTanks_VENDORED)
    add_subdirectory(vendored/sdl)
    set(SDL2IMAGE_VENDORED ON CACHE BOOL "Use all SDL libraries as vendored")
    set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "No need to build samples")
    set(SDL2IMAGE_INSTALL OFF CACHE BOOL "No need to install anywhere when we build it here anyway")
    add_subdirectory(vendored/sdl_image EXCLUDE_FROM_ALL)
    set(SDL2NET_INSTALL OFF CACHE BOOL "No need to install anywhere when we build it here anyway")
    add_subdirectory(vendored/sdl_net EXCLUDE_FROM_ALL)
    set(SDL2TTF_VENDORED ON CACHE BOOL "Use all SDL libraries as vendored")
    set(SDL2TTF_SAMPLES OFF CACHE BOOL "No need to build samples")
    set(SDL2TTF_INSTALL OFF CACHE BOOL "No need to install anywhere when we build it here anyway")
    add_subdirectory(vendored/sdl_ttf EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_net REQUIRED)
    find_package(SDL2_ttf REQUIRED)
endif()

# TODO: define subsystems as separate libraries and link to exe, so we may create test exes for them
add_executable(TypeTanks src/main.cpp)

if (TARGET SDL2::SDL2main)
    message(STATUS "SDL2main found")
    target_link_libraries(TypeTanks PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(TypeTanks PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_net::SDL2_net SDL2_ttf::SDL2_ttf)

# TODO: see if we can copy SDL-related DLLs to build directory so we can run the executable without manually copying shared libraries

# TODO: define tests
