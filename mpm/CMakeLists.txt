# MPK Package Manager
# Copyright 2013-2014 Josef Raschen <josef@raschen.org>
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

set(MPM_SRC
    main.c
)

set(MPM_LIBS
    m
    bz2
    tar
    ssl
    crypto
    mpk
)

add_executable(mpm ${MPM_SRC})
target_link_libraries(mpm ${MPM_LIBS})
