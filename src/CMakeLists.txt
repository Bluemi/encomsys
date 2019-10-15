file(GLOB_RECURSE encomsys_sources "./*.cpp")

add_library(encomsys ${encomsys_sources})

include_directories("/usr/include")

target_link_libraries(encomsys)
