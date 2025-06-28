set(BIN_NAME ${PROJECT_NAME}_TEST)

add_subdirectory(3rd/googletest-1.17.0)

file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${BIN_NAME} ${SOURCES})
target_link_libraries(${BIN_NAME} CC_Tools gtest gmock)
target_compile_definitions(${BIN_NAME} PUBLIC TEST_FILE="${CMAKE_CURRENT_SOURCE_DIR}/script/Tests.txt")

if (MSVC)
    target_compile_options(${BIN_NAME} PRIVATE /utf-8)
endif ()

include(GoogleTest)
gtest_discover_tests(${BIN_NAME})
