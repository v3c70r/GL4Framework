aux_source_directory(arcball SOURCES)
add_library (camera ${SOURCES})
target_link_libraries(camera core)
