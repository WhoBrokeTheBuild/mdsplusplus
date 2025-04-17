cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(mdsplusplus)

include(CTest)
include(FetchContent)

FetchContent_Declare(GSL
    GIT_REPOSITORY  "https://github.com/microsoft/GSL"
    GIT_TAG         "v4.2.0"
    GIT_SHALLOW     ON
)

FetchContent_MakeAvailable(GSL)

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
        Microsoft.GSL::GSL
)

target_include_directories(
    mdsplusplus
    PUBLIC
        include
)

set_target_properties(
    mdsplusplus
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

install(TARGETS mdsplusplus)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()