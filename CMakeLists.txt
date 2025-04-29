cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(mdsplusplus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()

find_package(MDSplus REQUIRED)

file(GLOB_RECURSE
    _source_list
    "source/*.h"
    "source/*.c"
    "source/*.hpp"
    "source/*.cpp"
)

add_library(
    mdsplusplus
    ${_source_list}
)

target_link_libraries(
    mdsplusplus
    PUBLIC
        MDSplus::MdsShr
        MDSplus::TdiShr
        MDSplus::TreeShr
        MDSplus::MdsIpShr
)

target_include_directories(
    mdsplusplus
    PUBLIC
        include
)

install(TARGETS mdsplusplus)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()