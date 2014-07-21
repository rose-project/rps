# Project mpk
# Copyright 2013-2014 Josef Raschen <josef@raschen.org>
project(mpk)
cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -ggdb -O0")

set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ext)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

file(GLOB LIBMPK_SRC 
    ${PROJECT_SOURCE_DIR}/mpk/*.c
    ${PROJECT_SOURCE_DIR}/include/mpk/*.h
    ${PROJECT_SOURCE_DIR}/mpk/*.h
)

set(LIBMPK_LIBS
    m
    bz2
    tar
    ssl
    crypto
    jansson
)

add_library(mpk SHARED ${LIBMPK_SRC} )
target_link_libraries(mpk ${LIBMPK_LIBS})

add_subdirectory(test)

add_subdirectory(mpm)

# generate API documentation

option(BUILD_DOCUMENTATION "Build Doxygen based API documentation" OFF)
if(BUILD_DOCUMENTATION)
    FIND_PACKAGE(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the API documentation.")
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
endif()
