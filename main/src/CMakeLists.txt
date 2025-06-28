set(BIN_NAME ${PROJECT_NAME})

file (GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if (${PROJECT_NAME}_BUILD_SHARED)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_library(${BIN_NAME} SHARED ${SOURCES})
else ()
    add_library(${BIN_NAME} STATIC ${SOURCES})
endif ()

target_include_directories(${BIN_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/3rd"
)

if (MSVC)
    target_compile_options(${BIN_NAME} PRIVATE /utf-8)
endif ()
