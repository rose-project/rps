# libmpk unit tests
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/mpk
)

link_directories(
)

add_definitions(-DTESTDATA_DIR="${PROJECT_SOURCE_DIR}/test/testdata/")

set(MPK_TEST_SRC
    main.cpp
)

set(MPK_TEST_LIBS
    mpk
    pthread
)

add_executable(libmpk-test ${MPK_TEST_SRC})
target_link_libraries(libmpk-test ${MPK_TEST_LIBS})


