cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(rps
    VERSION 0.2.0
    DESCRIPTION "ROSE Package Service Library and Tools"
)

add_compile_definitions(RPS_VERSION="${CMAKE_PROJECT_VERSION}")

find_package(LibArchive REQUIRED)


add_library(${PROJECT_NAME} SHARED
    lib/exception.cpp
    lib/file.cpp
    lib/manifest.cpp
    lib/package.cpp
    lib/stringhelper.h
    lib/stringhelper.cpp
    lib/version.cpp
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
)
target_link_libraries(${PROJECT_NAME} jansson archive)


add_executable(rps-client
    tools/rps-client.cpp
    tools/command.h
    tools/command.cpp
    tools/installcommand.h
    tools/installcommand.cpp
)
target_compile_features(rps-client PRIVATE cxx_std_17)
target_link_libraries(rps-client jansson rps)


add_executable(rps-package
    tools/rps-package.cpp
    tools/command.h
    tools/command.cpp
    tools/createcommand.h
    tools/createcommand.cpp
    tools/unpackcommand.h
    tools/unpackcommand.cpp
)
target_include_directories(rps-package PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_compile_features(rps-package PRIVATE cxx_std_17)
target_link_libraries(rps-package jansson rps)

install(TARGETS ${PROJECT_NAME} rps-client rps-package)

# clang format
file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND clang-format -i ${SOURCE_FILES}
    )
endif()

# unit tests
if(BUILD_TESTING)
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(rps-tests lib/test/main.cpp)
target_include_directories(rps-tests PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(rps-tests PRIVATE "TESTDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testdata\"")
target_link_libraries(rps-tests ${GTEST_LIBRARIES} rps)

gtest_discover_tests(rps-tests)
endif(BUILD_TESTING)
