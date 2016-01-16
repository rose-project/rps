
set(RPS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/rps)

set(RPS_SRC
    ${RPS_INCLUDE_DIR}/defines.h
    ${RPS_INCLUDE_DIR}/exception.h
    exception.cpp
    ${RPS_INCLUDE_DIR}/file.h
    file.cpp
    filehandling.h
    filehandling.cpp
    install.h
    install.cpp
    ${RPS_INCLUDE_DIR}/manifest.h
    manifest.cpp
    ${RPS_INCLUDE_DIR}/mpk.h
    mpk.cpp
    ${RPS_INCLUDE_DIR}/package.h
    package.cpp
    ${RPS_INCLUDE_DIR}/pkginfo.h
    pkginfo.cpp
    ${RPS_INCLUDE_DIR}/pkgref.h
    pkgref.cpp
    stringhelper.h
    stringhelper.cpp
    ${RPS_INCLUDE_DIR}/stringlist.h
    stringlist.cpp
    ${RPS_INCLUDE_DIR}/version.h
    version.cpp
)

set(RPS_LIBS
    stdc++fs
    bz2
    tar
    curl
    ssl
    crypto
    jansson
)

add_library(${PROJECT_NAME} SHARED ${RPS_SRC})
target_link_libraries(${PROJECT_NAME} ${RPS_LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

