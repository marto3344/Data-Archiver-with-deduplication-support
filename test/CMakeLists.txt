
add_executable(tests "tests.cpp")

target_include_directories(tests PRIVATE ../include)

target_link_libraries(tests PRIVATE backup_lib)

