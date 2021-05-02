cmake_minimum_required(VERSION 3.0.1)
project(rpmtrail)
include(ExternalProject)

set(LIBDNF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libdnf)
set(LIBDNF_LIB_INCLUDE ${LIBDNF_SOURCE_DIR}/include)

# libdnf
add_subdirectory(dependencies/libdnf)


include_directories( ${LIBDNF_LIB_INCLUDE} )
include_directories( ${LIBDNF_SOURCE_DIR} )

add_executable(rpmtrail
    src/main.cpp
)

#set(OpenGL_GL_PREFERENCE GLVND)
find_package(glfw3 3.3 REQUIRED)
#find_package(OpenGL REQUIRED)


find_package(Vulkan REQUIRED)
message(STATUS ${Vulkan_INCLUDE_DIR})
target_include_directories(rpmtrail PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(rpmtrail PUBLIC ${Vulkan_LIBRARIES})

target_link_libraries(rpmtrail PUBLIC glfw -lGL -lGLU -lglut)
#target_link_libraries(rpmtrail PUBLIC OpenGL::GL)

target_link_libraries(rpmtrail PUBLIC libdnf libdnf-cli)

set_target_properties(rpmtrail PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
add_dependencies(rpmtrail libdnf)
