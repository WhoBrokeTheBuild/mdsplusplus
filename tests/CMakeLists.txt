
file(GLOB
    _test_source_list
    "*.cpp"
)

foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            mdsplusplus
            GTest::gtest
    )

    set_target_properties(
        ${_test_name}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
    )

    add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}>
    )

endforeach()