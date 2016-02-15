aux_source_directory(imgui SOURCES)
aux_source_directory(stb SOURCES)
message(STATUS ${SOURCES})
add_library(third ${SOURCES})
