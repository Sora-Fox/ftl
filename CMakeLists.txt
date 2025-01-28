cmake_minimum_required(VERSION 3.24)
project(libftl LANGUAGES CXX)

add_library(ftl INTERFACE)
target_include_directories(
    ftl INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

option(FTL_ENABLE_TESTS "Enable building tests for FTL library" OFF)

if (FTL_ENABLE_TESTS)
    enable_testing()
    include(cmake/FetchGoogleTest.cmake)
    add_subdirectory(tests)
endif()

