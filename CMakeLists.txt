#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples

project(gl4framework)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_LIBRARY_ARCHITECTURE "x86_64-linux-gnu")
  endif()
endif()

cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
pkg_search_module(GLEW REQUIRED glew)

find_package(CUDA)

find_package (Threads)
pkg_search_module(GLFW REQUIRED glfw3)
#find_package(GLFW)
find_package(OpenGL)
find_package(assimp REQUIRED)
find_package(OpenMesh)
find_package(PythonLibs REQUIRED)
#find_package(DevIL)

#Adding brew dirs
include_directories(/usr/local/include/)
include_directories(./src/third_party)
include_directories(./src/lib)
IF (${CUDA_FOUND})
    include_directories(${CUDA_INCLUDE_DIRS})
ENDIF (${CUDA_FOUND})


set(CMAKE_CXX_FLAGS "--std=c++14 -g -Wall")

#Setting up include libraries


#file(GLOB ALL_FILES
#    "./imgui/*.h"
#    "./imgui/*.cpp"
#    "*.h"
#    "*.hpp"
#    "*.cpp"
#    )
add_subdirectory(./src/lib/core)
add_subdirectory(./src/third_party)
add_subdirectory(./src/lib/camera)
add_subdirectory(./src/lib/objects)
add_subdirectory(./src/lib/renderers)
add_subdirectory(./src/lib/console)


add_subdirectory(./src/app)
#=== Detecting CUDA
#IF (${CUDA_FOUND})
#    cuda_compile(POINTS_O points.cu OPTIONS --std=c++11)
#    cuda_add_executable(main ${ALL_FILES} ${POINTS_O})
#ELSE (${CUDA_FOUND})
#    add_executable(main ${ALL_FILES})
#ENDIF (${CUDA_FOUND})


#target_link_libraries(main ${GLFW_LIBRARIES})
#message(STATUS "OPENMESH_LIBRARIES = ${OPENMESH_LIBRARIES}")
#target_link_libraries(main ${OPENMESH_LIBRARIES})
#target_link_libraries(main ${OPENGL_LIBRARIES})
#target_link_libraries(main ${GLEW_LIBRARIES})
#target_link_libraries(main ${ASSIMP_LIBRARIES})
#target_link_libraries(main ${PYTHON_LIBRARIES})
#target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(main ${IL_LIBRARIES})
#target_link_libraries(main ${ILU_LIBRARIES})
#target_link_libraries(main dl)

#Copy files to current directory
