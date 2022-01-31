# Project ROSE Package Service
# Copyright 2013, 2015 Josef Raschen
project(rps)
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

find_package(LibArchive REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${LibArchive_INCLUDE_DIRS}
)

link_directories(

)

add_subdirectory(src/test)
add_subdirectory(src/rps)
add_subdirectory(src/rps-tools)

install(DIRECTORY include/rps DESTINATION include)
