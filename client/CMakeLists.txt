cmake_minimum_required(VERSION 3.0.0)
project(TypeTanks VERSION 1.0.0)

option(TypeTanks_VENDORED "Use vendored libraries" OFF)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(TypeTanks_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
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

add_library(tt_event STATIC
    src/events/EventSink.cpp)
target_compile_features(tt_event PUBLIC cxx_std_20)
target_include_directories(tt_event PUBLIC src)

add_library(tt_script STATIC
    src/script/FileFollower.cpp
    src/script/ScriptParser.cpp)
target_compile_features(tt_script PUBLIC cxx_std_20)
target_include_directories(tt_script PUBLIC src)
target_link_libraries(tt_script PUBLIC tt_event)

add_library(tt_logic STATIC
    src/logic/CoreLogic.cpp
    src/logic/DummyLogic.cpp)
target_compile_features(tt_logic PUBLIC cxx_std_20)
target_include_directories(tt_logic PUBLIC src)
target_link_libraries(tt_logic PUBLIC tt_event)

add_library(tt_comms STATIC
    src/net/CommParser.cpp)
target_compile_features(tt_comms PUBLIC cxx_std_20)
target_include_directories(tt_comms PUBLIC src src/utility/rapidjson/include)
target_link_libraries(tt_comms PUBLIC tt_event)

add_library(tt_graphics STATIC
    src/graphics/Graphics.cpp)
target_compile_features(tt_graphics PUBLIC cxx_std_20)
target_include_directories(tt_graphics PUBLIC src src/utility/rapidjson/include)
target_link_libraries(tt_graphics PUBLIC tt_event SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2::SDL2)

# TODO: define subsystems as separate libraries and link to exe, so we may create test exes for them
add_executable(TypeTanks src/main.cpp src/net/Connection.cpp)
target_compile_features(TypeTanks PUBLIC cxx_std_20)

if (TARGET SDL2::SDL2main)
    message(STATUS "SDL2main found")
    target_link_libraries(TypeTanks PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(TypeTanks
    PRIVATE 
        SDL2::SDL2
        SDL2_net::SDL2_net
        tt_script
        tt_logic
        tt_comms
        tt_graphics)

# TODO: see if we can copy SDL-related DLLs to build directory so we can run the executable without manually copying shared libraries

# TODO: define tests
enable_testing()

add_executable(test_eventqueue tests/events/TestEventQueue.cpp)
target_include_directories(test_eventqueue PRIVATE tests)
target_link_libraries(test_eventqueue PRIVATE tt_event)

add_test(NAME EventQueue COMMAND test_eventqueue)

add_executable(test_scriptparser tests/script/TestScriptParser.cpp)
target_include_directories(test_scriptparser PRIVATE tests)
target_link_libraries(test_scriptparser PRIVATE tt_script)

add_test(NAME ScriptParser COMMAND test_scriptparser)

add_executable(test_corelogic tests/logic/TestCoreLogic.cpp)
target_include_directories(test_corelogic PRIVATE tests)
target_link_libraries(test_corelogic PRIVATE tt_logic)

add_test(NAME CoreLogic COMMAND test_corelogic)

add_executable(test_comms tests/net/TestCommParser.cpp)
target_include_directories(test_comms PRIVATE tests)
target_link_libraries(test_comms PRIVATE tt_comms)

add_test(NAME Comms COMMAND test_comms)

add_executable(run_graphics tests/graphics/RunGraphics.cpp)
target_link_libraries(run_graphics PRIVATE tt_graphics)

if (TARGET SDL2::SDL2main)
    target_link_libraries(run_graphics PRIVATE SDL2::SDL2main)
endif()
