aux_source_directories(SOURCE     .)
aux_source_directories(SOURCE     ./core)
aux_source_directories(SOURCE     ./cameras)
aux_source_directories(SOURCE     ./objects)
aux_source_directories(SOURCE     ./renderers)

add_library(basic ${SOURCE})


