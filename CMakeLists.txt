cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(mdsplusplus CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()

find_package(MDSplus REQUIRED)
find_package(Python COMPONENTS Interpreter REQUIRED)

file(GLOB_RECURSE
    _source_list
    "source/*.hpp"
)

set(_single_header ${CMAKE_SOURCE_DIR}/include/mdsplus.hpp)
set(_test_source ${CMAKE_SOURCE_DIR}/source/mdsplus.cpp)

add_custom_command(
    OUTPUT ${_single_header}
    DEPENDS ${_source_list}
    COMMAND ${Python_EXECUTABLE} scripts/gen-single-header.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(
    mdsplusplus-test
    ${_single_header}
    ${_test_source}
)

target_include_directories(
    mdsplusplus-test
    PRIVATE
        source
)

target_link_libraries(
    mdsplusplus-test
    PRIVATE
        MDSplus::MdsShr
        MDSplus::TdiShr
        MDSplus::TreeShr
        MDSplus::MdsIpShr
)

add_library(mdsplusplus INTERFACE)

target_sources(
    mdsplusplus
    INTERFACE
        ${CMAKE_SOURCE_DIR}/include/mdsplus.hpp
)

target_link_libraries(
    mdsplusplus
    INTERFACE
        MDSplus::MdsShr
        MDSplus::TdiShr
        MDSplus::TreeShr
        MDSplus::MdsIpShr
)

target_include_directories(
    mdsplusplus
    INTERFACE
        include
)

install(TARGETS mdsplusplus)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()