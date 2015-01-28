# libmpk unit tests
# Copyright 2013 Josef Raschen <josef@raschen.org>
cmake_minimum_required(VERSION 2.8)

ExternalProject_Add(
    googletest
    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
    TIMEOUT 60
    INSTALL_COMMAND ""
)

include_directories(
    ${PROJECT_BINARY_DIR}/ext/src/googletest/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/mpk
)

link_directories(
    ${PROJECT_BINARY_DIR}/ext/src/googletest-build
)

add_definitions(-DTESTDATA_DIR="${PROJECT_SOURCE_DIR}/test/testdata/")

set(MPK_TEST_SRC
    main.cpp
)

set(MPK_TEST_LIBS
    mpk
    gtest
    pthread
    gtest_main
)

add_executable(libmpk-test ${MPK_TEST_SRC})
target_link_libraries(libmpk-test ${MPK_TEST_LIBS})

add_dependencies(libmpk-test googletest)

add_custom_target(test libmpk-test)

