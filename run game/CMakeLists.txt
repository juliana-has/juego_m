cmake_minimum_required(VERSION 3.16)
project(Run VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Busca ncurses
find_package(Curses REQUIRED)

include_directories(include ${CURSES_INCLUDE_DIR})

set(SOURCES
    src/main.cpp
    src/map.cpp
    src/entity.cpp
    src/renderer.cpp
)

add_executable(run_game ${SOURCES})
target_link_libraries(run_game ${CURSES_LIBRARIES})

# Warnings útiles
target_compile_options(run_game PRIVATE -Wall -Wextra -O2)
