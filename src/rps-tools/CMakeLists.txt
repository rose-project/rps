set(RPS_CLIENT_SRC
    rps-client.cpp
    command.h
    command.cpp
    installcommand.h
    installcommand.cpp
)

set(RPS_PKGTOOL_SRC
    rps-package.cpp
    command.h
    command.cpp
    createcommand.h
    createcommand.cpp
    unpackcommand.h
    unpackcommand.cpp
)

set(RPS_TOOLS_LIBS
    rps
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${LibArchive_LIBRARY}
)

add_executable(rps-client ${RPS_CLIENT_SRC})
set_property(TARGET rps-client PROPERTY CXX_STANDARD 14)
target_link_libraries(rps-client ${RPS_TOOLS_LIBS})

add_executable(rps-package ${RPS_PKGTOOL_SRC})
set_property(TARGET rps-package PROPERTY CXX_STANDARD 14)
target_link_libraries(rps-package ${RPS_TOOLS_LIBS})

install(TARGETS rps-client rps-package DESTINATION bin)
