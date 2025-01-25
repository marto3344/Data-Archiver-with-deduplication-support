cmake_minimum_required(VERSION 3.16)
project(backup VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(backup ${SOURCES})

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_library(backup_lib STATIC ${SOURCES})
    target_link_libraries(backup PRIVATE backup_lib)
    add_subdirectory(test)
endif()