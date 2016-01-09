# Project ROSE Package Service
# Copyright 2013, 2015 Josef Raschen
project(rps)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

add_subdirectory(src/test)
add_subdirectory(src/rps)
add_subdirectory(src/rps-tools)

install(DIRECTORY include/rps DESTINATION include)
