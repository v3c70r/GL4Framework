#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples
cmake_minimum_required(VERSION 3.3)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
find_package(OpenGL)
find_package(assimp)
find_package(pythonLibs)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(OpenMesh)

#Adding brew dirs
include_directories(/usr/local/include/)
link_directories(/usr/local/lib)

set(CMAKE_CXX_FLAGS "--std=c++11 -g -Wall")

file(GLOB ALL_FILES
    "./imgui/*.h"
    "./imgui/*.cpp"
    "*.h"
    "*.hpp"
    "*.cpp"
    )

add_executable(main ${ALL_FILES})
target_link_libraries(main ${GLFW_LIBRARIES})
target_link_libraries(main ${OPENGL_LIBRARIES})
target_link_libraries(main ${GLEW_LIBRARIES})
target_link_libraries(main ${ASSIMP_LIBRARIES})
target_link_libraries(main ${ASSIMP_LIBRARIES})
target_link_libraries(main ${PYTHON_LIBRARIES})

#Copy files to current directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    shaders)
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/meshes
    meshes)




