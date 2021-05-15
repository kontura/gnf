cmake_minimum_required(VERSION 3.5)
project(gnf)

find_package(PkgConfig REQUIRED)

set(LIBDNF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libdnf)
set(LIBDNF_LIB_INCLUDE ${LIBDNF_SOURCE_DIR}/include)

set(WITH_RUBY OFF CACHE BOOL "LIBDNF option")
set(WITH_GO OFF CACHE BOOL "LIBDNF option")
set(WITH_PYTHON3 OFF CACHE BOOL "LIBDNF option")
set(WITH_PERL5 OFF CACHE BOOL "LIBDNF option")
set(WITH_TESTS OFF CACHE BOOL "LIBDNF option")
set(WITH_MAN OFF CACHE BOOL "LIBDNF option")
set(WITH_HTML OFF CACHE BOOL "LIBDNF option")

set(WITH_LIBDNF_CLI OFF CACHE BOOL "LIBDNF option")
set(WITH_MICRODNF OFF CACHE BOOL "LIBDNF option")
set(WITH_PYTHON_PLUGINS_LOADER OFF CACHE BOOL "LIBDNF option")
set(WITH_DNFDAEMON_CLIENT OFF CACHE BOOL "LIBDNF option")
set(WITH_DNFDAEMON_SERVER OFF CACHE BOOL "LIBDNF option")
# libdnf
add_subdirectory(dependencies/libdnf)

include_directories( ${LIBDNF_LIB_INCLUDE} )
include_directories( ${LIBDNF_SOURCE_DIR} )

add_executable(gnf
    src/main.cpp
)

pkg_check_modules(GLFW3 REQUIRED glfw3)
target_link_libraries(gnf ${GLFW3_LIBRARIES})

pkg_check_modules(GLEW REQUIRED glew)
target_link_libraries(gnf ${GLEW_LIBRARIES})

target_link_libraries(gnf libdnf)

set_target_properties(gnf PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
add_dependencies(gnf libdnf)
