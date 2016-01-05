
set(RPS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/rps)

set(RPS_SRC
    ${RPS_INCLUDE_DIR}/defines.h
    ${RPS_INCLUDE_DIR}/file.h
    file.c
    filehandling.h
    filehandling.c
    install.h
    install.c
    ${RPS_INCLUDE_DIR}/manifest.h
    manifest.c
    ${RPS_INCLUDE_DIR}/mpk.h
    mpk.c
    package.h
    package.c
    ${RPS_INCLUDE_DIR}/pkginfo.h
    pkginfo.c
    ${RPS_INCLUDE_DIR}/pkgref.h
    pkgref.c
    stringhelper.h
    stringhelper.c
    ${RPS_INCLUDE_DIR}/stringlist.h
    stringlist.c
    ${RPS_INCLUDE_DIR}/version.h
    version.c
)

set(RPS_LIBS
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

