add_executable(MainTest main_test.cpp)
add_executable(VecTest index_vec_test.cpp)
add_executable(RelationTest relation_test.cpp)
add_executable(RudiTest rudi_example.cpp)

include_directories(ENCOMSYS_SOURCE_DIR)

target_link_libraries(MainTest "encomsys")
target_link_libraries(VecTest "encomsys")
target_link_libraries(RelationTest "encomsys")
target_link_libraries(RudiTest "encomsys")
