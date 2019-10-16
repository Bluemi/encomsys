add_executable(MainTest main_test.cpp)
add_executable(VecTest holey_vec_test.cpp)

include_directories(ENCOMSYS_SOURCE_DIR)

target_link_libraries(MainTest "encomsys")
target_link_libraries(VecTest "encomsys")
