# MPK Package Manager
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
)

set(MPM_SRC
    main.cpp
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
