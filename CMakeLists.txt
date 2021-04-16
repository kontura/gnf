cmake_minimum_required(VERSION 3.0.1)
project(rpmtrail)
include(ExternalProject)

# SourcetrailDB
set(SOURCETRAILDB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SourcetrailDB)
set(SOURCETRAILDB_LIB_INCLUDE ${SOURCETRAILDB_SOURCE_DIR}/core/include)
set(SOURCETRAILDB_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/SourcetrailDB-build/core)
ExternalProject_add(SourcetrailDB
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    GIT_SUBMODULES dependencies/SourcetrailDB
    SOURCE_DIR ${SOURCETRAILDB_SOURCE_DIR}
    INSTALL_COMMAND ""
)
include_directories( ${SOURCETRAILDB_LIB_INCLUDE} )
add_library(sourcetraildb SHARED IMPORTED)
set_target_properties(sourcetraildb PROPERTIES
    IMPORTED_LOCATION ${SOURCETRAILDB_LIB_DIR}/libsourcetraildb.a)

# rpmtrail

add_executable(rpmtrail
    src/main.cpp
)

target_link_libraries(rpmtrail
    PRIVATE sourcetraildb
    -pthread
    -ldl)

set_target_properties(rpmtrail PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
add_dependencies(rpmtrail SourcetrailDB)
