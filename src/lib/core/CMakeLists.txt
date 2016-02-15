aux_source_directory(./ SOURCES)

add_library(core ${SOURCES})
target_link_libraries(core ${GLFW_STATIC_LIBRARIES} GLEW IL ILU assimp)
#target_link_libraries(core third)
#target_link_libraries(core ${GLFW_LIBRARIES})
#target_link_libraries(core ${PYTHON_LIBRARIES})

