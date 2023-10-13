cmake_minimum_required(VERSION 3.0.0)
project(TypeTank VERSION 1.0.0)

option(TypeTank_VENDORED "Use vendored libraries" OFF)

if(TypeTank_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()

# TODO: define subsystems as separate libraries and link to exe, so we may create test exes for them
add_executable(TypeTank src/main.cpp)

if (TARGET SDL2::SDL2main)
    target_link_libraries(TypeTank PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(TypeTank PRIVATE SDL2::SDL2)

# TODO: define tests
