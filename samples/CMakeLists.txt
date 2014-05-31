# some libmpk samples
# Copyright 2014 Josef Raschen <josef@raschen.org>
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

add_definitions(-DTESTDATA_DIR="${PROJECT_SOURCE_DIR}/test/testdata/")

link_directories(
    ${LIBMPK_LIB_DIR}
)

set(MPK_SAMPLES_LIBS
    mpk
)

add_executable(mpk-client mpk_client.c)
target_link_libraries(mpk-client ${MPK_SAMPLES_LIBS})
