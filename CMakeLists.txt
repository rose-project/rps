# Project ROSE Package Service
# Copyright 2013, 2015 Josef Raschen
project(rps)
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(CXX_EXTENSIONS )

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

add_subdirectory(src/test)
add_subdirectory(src/rps)
add_subdirectory(src/rps-tools)

install(DIRECTORY include/rps DESTINATION include)
