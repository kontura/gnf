cmake_minimum_required(VERSION 3.5)
project(gnf)

find_package(PkgConfig REQUIRED)

add_executable(gnf
    src/main.cpp
    src/gui.cpp
    src/package_layout.cpp
    src/package_graph.cpp
)

pkg_check_modules(GLFW3 REQUIRED glfw3)
target_link_libraries(gnf ${GLFW3_LIBRARIES})

pkg_check_modules(GLEW REQUIRED glew)
target_link_libraries(gnf ${GLEW_LIBRARIES})

pkg_check_modules(LIBDNF5 REQUIRED libdnf5)
target_link_libraries(gnf ${LIBDNF5_LIBRARIES})

set_target_properties(gnf PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
