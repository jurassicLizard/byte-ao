function(add_unit_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE jlizard::byte-ao)
    add_test(NAME "${TEST_NAME}" COMMAND ${TEST_NAME})
endfunction()

# the funciton above is currently unused

add_executable(byteao_unit_tests tests/unit_tests.cpp)
target_link_libraries(byteao_unit_tests PRIVATE jlizard::byte-ao)
#target_include_directories(byteao_unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")

add_test(NAME "Byte Array Ops Unit Tests" COMMAND byteao_unit_tests)