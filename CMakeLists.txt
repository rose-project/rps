# Project mpk
# Copyright 2013-2014 Josef Raschen <josef@raschen.org>
project(mpk)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic -Wextra")

add_subdirectory(ext/gtest)

include_directories(
    ${PROJECT_SOURCE_DIR}/include/mpk
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

file(GLOB LIBMPK_SRC 
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
)

set(LIBMPK_LIBS
    m
    bz2
    tar
    ssl
    crypto
    yaml
)

add_library(mpk SHARED ${LIBMPK_SRC} )
target_link_libraries(mpk ${LIBMPK_LIBS})

add_subdirectory(test)
