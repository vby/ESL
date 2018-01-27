cmake_minimum_required(VERSION 2.8)

project(ESL_tests CXX)

set(BUILD_GTEST ON)
set(BUILD_GMOCK OFF)
add_subdirectory(googletest)

function(add_esl_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} esl gtest gtest_main)
	add_test(NAME ${name} COMMAND ${name})
endfunction()

add_esl_test(span_tests)

