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
    ${LibArchive_LIBRARY}
)

add_executable(rps-client ${RPS_CLIENT_SRC})
target_compile_features(rps-client PRIVATE cxx_std_17)
target_link_libraries(rps-client ${RPS_TOOLS_LIBS})

add_executable(rps-package ${RPS_PKGTOOL_SRC})
target_compile_features(rps-package PRIVATE cxx_std_17)
target_link_libraries(rps-package ${RPS_TOOLS_LIBS})

install(TARGETS rps-client rps-package DESTINATION bin)
