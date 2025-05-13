add_executable(byteao_unit_tests tests/unit_tests.cpp)
target_link_libraries(byteao_unit_tests PRIVATE jlizard::byte-ao)
#target_include_directories(byteao_unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")

add_test(NAME "Byte Array Ops Unit Tests" COMMAND byteao_unit_tests)