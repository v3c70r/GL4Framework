aux_source_directory(SOURCES  .)

add_executable(gl4framework App.cpp main.cpp pyConsole.cpp)

FOREACH(item ${OPENGL_LIBRARIES})
    MESSAGE(STATUS ${item})
ENDFOREACH(item)

target_link_libraries(gl4framework 
    third 
    core 
    objs  
    renderer 
    camera 
    pthread 
    python2.7
    ${OPENGL_LIBRARIES}
    )


IF (${CUDA_FOUND})
    add_definitions(-CUDA)
    target_link_libraries(gl4framework cuda_obj)
ENDIF (${CUDA_FOUND})

set_target_properties ( gl4framework
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

add_custom_command(TARGET gl4framework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_BINARY_DIR}/bin/shaders)
add_custom_command(TARGET gl4framework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/bin/assets)
add_custom_command(TARGET gl4framework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/scripts
    ${CMAKE_BINARY_DIR}/bin/scripts)




