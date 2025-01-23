cmake_minimum_required(VERSION 3.16)
project(backup VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

add_library(backup_lib STATIC ${SOURCES})

add_executable(backup_app ${SOURCES})

set_target_properties(backup_app PROPERTIES OUTPUT_NAME "backup")

target_link_libraries(backup_app PRIVATE backup_lib)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()