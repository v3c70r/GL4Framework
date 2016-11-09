#aux_source_directory(compMesh SOURCES)
#aux_source_directory(defMeshNode SOURCES)
#aux_source_directory(meshNode SOURCES)
set (SOURCES
    meshNode.cpp
    #defMeshNode.cpp
    )
#aux_source_directory(GLSPH SOURCES)


#IF (${CUDA_FOUND})
#    message(STATUS "Found CUDA")
#    message(STATUS ${CUDA_INCLUDE_DIRS})
#    #aux_source_directory(point CUDA_SOURCES)
#    file(GLOB CUDA_SOURCES 
#        "point/points.*"
#        )
#    message(STATUS ${CUDA_SOURCES})
#    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;-std=c++11;-O2;-DVERBOSE")
#    message(STATUS "NVCC FLAGS ${CUDA_NVCC_FLAGS}")
#    cuda_compile(POINTS_O points.cu )
#    cuda_add_library(cuda_obj ${CUDA_SOURCES})
#ENDIF (${CUDA_FOUND})
add_library(objs ${SOURCES})
target_link_libraries(objs core)
