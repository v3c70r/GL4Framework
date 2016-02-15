#aux_source_directory(compMesh SOURCES)
aux_source_directory(defMeshNode SOURCES)
aux_source_directory(meshNode SOURCES)


IF (${CUDA_FOUND})
    #message(STATUS "Found CUDA")
    #message(STATUS ${CUDA_INCLUDE_DIRS})
    ##aux_source_directory(point CUDA_SOURCES)
    #file(GLOB CUDA_SOURCES 
    #    "point/point.*"
    #    )
    #message(STATUS ${CUDA_SOURCES})
    ##cuda_compile(POINTS_O points.cu OPTIONS --std=c++11)
    #cuda_add_library(cuda_obj ${CUDA_SOURCES})
ENDIF (${CUDA_FOUND})
add_library(objs ${SOURCES})
target_link_libraries(objs core)
