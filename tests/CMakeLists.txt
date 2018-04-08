cmake_minimum_required(VERSION 2.8)

project(ESL_tests CXX)

set(BUILD_GTEST ON CACHE BOOL "")
set(BUILD_GMOCK OFF CACHE BOOL "")
set(INSTALL_GTEST OFF CACHE BOOL "")
set(gtest_force_shared_crt ON CACHE BOOL "")
add_subdirectory(googletest)

function(esl_add_executable target name)
    add_executable(${target} ${name}.cpp)
    target_link_libraries(${target} ESL gtest gtest_main)
	target_compile_options(${target} PRIVATE
				$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
				$<$<CXX_COMPILER_ID:GNU>:-Wall -W -Werror -Wpedantic>
				$<$<CXX_COMPILER_ID:Clang>:-Wall -W -Werror -Wpedantic>
			)
endfunction()

function(esl_add_shared name tests_target)
	add_library(${name} SHARED ${name}.cpp)
	target_link_libraries(${name} ESL)
	target_link_libraries(${tests_target} ${CMAKE_DL_LIBS})
	add_dependencies(${tests_target} ${name})
	target_compile_definitions(${tests_target} PRIVATE "ESL_TESTS_DL_FILENAME=$<TARGET_FILE:${name}>")
endfunction()
	
function(esl_add_test name)
	esl_add_executable(${name} ${name})
	add_test(NAME ${name} COMMAND ${name})
endfunction()

set(CMAKE_CXX_EXTENSIONS OFF)

esl_add_test(utility_tests)
esl_add_test(type_traits_tests)
esl_add_test(source_location_tests)
esl_add_test(array_tests)
esl_add_test(functional_tests)
esl_add_test(span_tests)
esl_add_test(string_tests)
esl_add_test(endian_tests)
esl_add_test(map_utils_tests)
esl_add_test(linked_list_tests)
esl_add_test(lazy_tests)
esl_add_test(codecs_tests)
esl_add_test(unicode_tests)
esl_add_test(demangle_tests)
esl_add_test(flex_tests)
esl_add_test(flex_variant_tests)
esl_add_test(json_tests)
esl_add_test(yaml_tests)
esl_add_test(shared_library_tests)

esl_add_shared(shared_library_tests_dl shared_library_tests)

