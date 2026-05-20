cmake_minimum_required(VERSION 3.10)
project(AmorGame CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(SOURCES
    main.cpp
    src/map.cpp
    src/player.cpp
    src/enemy.cpp
    src/items.cpp
    src/input.cpp
    src/renderer.cpp
    src/game.cpp
    src/audio.cpp
    src/narrative.cpp
)

add_executable(amor_game ${SOURCES})
target_include_directories(amor_game PRIVATE ${CMAKE_SOURCE_DIR})

if(UNIX)
    target_compile_definitions(amor_game PRIVATE _POSIX_C_SOURCE=200809L)
    # miniaudio necesita pthread y dl en Linux
    target_link_libraries(amor_game pthread dl m)
endif()

if(WIN32)
    target_link_libraries(amor_game winmm)
endif()
