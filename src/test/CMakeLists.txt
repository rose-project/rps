# librps unit tests

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
set_property(TARGET librps-test PROPERTY CXX_STANDARD 14)
target_link_libraries(librps-test ${RPS_TEST_LIBS})


