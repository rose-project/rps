set(RPS_CLIENT_SRC
        rps-client.cpp
)

set(RPS_CREATE_SRC
        rps-create.cpp
)

set(RPS_TOOLS_LIBS
    rps
)

add_executable(rps-client ${RPS_CLIENT_SRC})
target_link_libraries(rps-client ${RPS_TOOLS_LIBS})

add_executable(rps-create ${RPS_CREATE_SRC})
target_link_libraries(rps-create ${RPS_TOOLS_LIBS})
