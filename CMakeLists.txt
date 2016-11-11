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

find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(CUDA)
find_package(Threads REQUIRED)
find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenMesh)
find_package(PythonLibs REQUIRED)
find_package(DevIL REQUIRED)

#Adding brew dirs
include_directories(/usr/local/include/)
include_directories(./src/third_party)
include_directories(./src/lib)

IF (${CUDA_FOUND})
    include_directories(${CUDA_INCLUDE_DIRS})
ENDIF (${CUDA_FOUND})

set(CMAKE_CXX_FLAGS "--std=c++11 -g -Wall")

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
add_subdirectory(./src/app)
