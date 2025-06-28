cmake_minimum_required(VERSION 3.20.0)

project(CC_Tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(${PROJECT_NAME}_BUILD_TESTS "Build tests" ON)
option(${PROJECT_NAME}_BUILD_SHARED "Build shared" OFF)

enable_testing()

add_subdirectory(main/src)

if (${PROJECT_NAME}_BUILD_TESTS)
    add_subdirectory(main/test)
endif ()
