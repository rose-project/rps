# Project mpk
# Copyright 2013-2014 Josef Raschen <josef@raschen.org>
project(mpk)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic -Wextra")

add_subdirectory(ext/gtest)
add_subdirectory(ext/jsmn)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/ext/jsmn
)

link_directories(
)

file(GLOB LIBMPK_SRC 
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/include/mpk/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
)

set(LIBMPK_LIBS
    m
    bz2
    tar
    ssl
    crypto
    jsmn
    jansson
)

add_library(mpk SHARED ${LIBMPK_SRC} )
target_link_libraries(mpk ${LIBMPK_LIBS})

add_subdirectory(test)

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
