file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(tests ${TEST_SOURCES})

target_include_directories(tests PRIVATE ../include)

target_link_libraries(tests PRIVATE backup_lib)

find_package(Threads REQUIRED)
target_link_libraries(tests PRIVATE Threads::Threads)