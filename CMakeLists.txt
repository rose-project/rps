# Project libmpk
# Copyright 2013 Josef Raschen <josef@raschen.org>
project(libmpk)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic -Wextra")

set(LIBMPK_SRC
    defines.h
    err.h
    mpk.c
    config.c
    pkginfo.c
    file.c
    stringlist.c
    manifest.c
    pkgref.c
    version.c
    package.c
)

set(LIBMPK_LIBS
    bz2
    ssl
    yaml
)

add_library(mpk SHARED ${LIBMPK_SRC})
target_link_libraries(mpk ${LIBMPK_LIBS})

