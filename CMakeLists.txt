# Project libmpk
# Copyright 2013 Josef Raschen <josef@raschen.org>
project(libmpk)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic -Wextra")

set(LIBMPK_SRC
    defines.h
    err.h
    mpk.c
    config.c
    pkginfo.c
    file.c
    stringhelper.c
    stringlist.c
    manifest.c
    pkgref.c
    version.c
    package.c
)

set(LIBMPK_LIBS
    m
    bz2
    ssl
    crypto
    yaml
)

add_library(mpk SHARED ${LIBMPK_SRC})
target_link_libraries(mpk ${LIBMPK_LIBS})

