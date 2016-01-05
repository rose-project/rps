# librps unit tests
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/rps
)

link_directories(
)

add_definitions(-DTESTDATA_DIR="${PROJECT_SOURCE_DIR}/testdata/")

set(RPS_TEST_SRC
    main.cpp
)

set(RPS_TEST_LIBS
    pthread
)

add_executable(librps-test ${RPS_TEST_SRC})
target_link_libraries(librps-test ${RPS_TEST_LIBS})


