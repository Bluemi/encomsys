add_executable(MainTest main_test.cpp)

include_directories(ENCOMSYS_SOURCE_DIR)

target_link_libraries(MainTest "encomsys")
