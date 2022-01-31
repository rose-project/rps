
set(RPS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/rps)

set(RPS_SRC
    ${RPS_INCLUDE_DIR}/defines.h
    ${RPS_INCLUDE_DIR}/exception.h
    exception.cpp
    ${RPS_INCLUDE_DIR}/file.h
    file.cpp
    ${RPS_INCLUDE_DIR}/manifest.h
    manifest.cpp
    ${RPS_INCLUDE_DIR}/mpk.h
    ${RPS_INCLUDE_DIR}/package.h
    package.cpp
    stringhelper.h
    stringhelper.cpp
    ${RPS_INCLUDE_DIR}/version.h
    version.cpp
)

set(RPS_LIBS
    jansson
    ${LibArchive_LIBRARY}
)

set(RPS_VERSION_MAJOR 0)
set(RPS_VERSION_MINOR 2)
set(RPS_VERSION_PATCH 0)
set(RPS_VERSION_STRING ${RPS_VERSION_MAJOR}.${RPS_VERSION_MINOR}.${RPS_VERSION_PATCH})

add_library(${PROJECT_NAME} SHARED ${RPS_SRC})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${RPS_VERSION_STRING}
                                          SOVERSION ${RPS_VERSION_MAJOR})

target_link_libraries(${PROJECT_NAME} ${RPS_LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

