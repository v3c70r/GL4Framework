aux_source_directory(./ SOURCES)

message(STATUS "ASSIMP LIB " ${assimp_LIBRARIES})
add_library(core ${SOURCES})
target_link_libraries(core 
    ${GLFW_LIBRARIES} 
    ${GLEW_LIBRARIES}
    ${DevIL_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${assimp_LIBRARIES}
    )

#target_link_libraries(core third)
#target_link_libraries(core ${GLFW_LIBRARIES})
#target_link_libraries(core ${PYTHON_LIBRARIES})

