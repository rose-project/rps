set(RPS_CLIENT_SRC
        main.cpp
)

set(RPS_CLIENT_LIBS
    rps
)

add_executable(rps-client ${RPS_CLIENT_SRC})
target_link_libraries(rps-client ${RPS_CLIENT_LIBS})
