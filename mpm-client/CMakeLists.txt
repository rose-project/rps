set(MPM_CLIENT_SRC
        main.c
)

set(MPM_CLIENT_LIBS
    mpk
)

add_executable(mpm-client ${MPM_CLIENT_SRC})
target_link_libraries(mpm-client ${MPM_CLIENT_LIBS})
