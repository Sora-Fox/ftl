function(enable_sanitizers_if_debug TARGET)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND
        CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(
                ${TARGET} PRIVATE
                -fsanitize=address,undefined,leak
                -fno-omit-frame-pointer
            )
            target_link_options(
                ${TARGET} PRIVATE
                -fsanitize=address,undefined,leak
            )
    endif()
endfunction()

function(add_gtest TARGET SRC)
    add_executable(${TARGET} ${SRC})
    target_link_libraries(
        ${TARGET} PRIVATE
        ftl
        GTest::gtest_main
    )
    enable_sanitizers_if_debug(${TARGET})
    gtest_discover_tests(${TARGET})
endfunction()

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vector_test.cpp
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_gtest(${TEST_NAME} ${TEST_FILE})
endforeach()

