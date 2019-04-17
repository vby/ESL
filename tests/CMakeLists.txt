cmake_minimum_required(VERSION 3.11)

project(ESL_tests CXX)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.8.1
        GIT_PROGRESS TRUE
    )
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    set(BUILD_GTEST ON CACHE BOOL "")
    set(BUILD_GMOCK OFF CACHE BOOL "")
    set(INSTALL_GTEST OFF CACHE BOOL "")
    set(gtest_force_shared_crt ON CACHE BOOL "")
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# tests
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MACOSX_RPATH TRUE)

function(esl_add_shared name tests_target)
	add_library(${name} SHARED ${name}.cpp)
	target_link_libraries(${name} ESL)
	target_link_libraries(${tests_target} ${CMAKE_DL_LIBS})
	add_dependencies(${tests_target} ${name})
	target_compile_definitions(${tests_target} PRIVATE "ESL_TESTS_DL_FILENAME=$<TARGET_FILE:${name}>")
endfunction()
	
function(esl_add_test name)
    set(target ${name}_tests)
    add_executable(${target} ${target}.cpp ../esl/${name}.hpp)
    target_link_libraries(${target} ESL gtest gtest_main)
	target_compile_options(${target} PRIVATE
				$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
				$<$<CXX_COMPILER_ID:GNU>:-Wall -W -Werror -Wpedantic>
				$<$<CXX_COMPILER_ID:Clang>:-Wall -W -Werror -Wpedantic>
			)
	add_test(NAME ${target} COMMAND ${target})
endfunction()

esl_add_test(utility)
esl_add_test(type_traits)
esl_add_test(source_location)
esl_add_test(array)
esl_add_test(functional)
esl_add_test(span)
esl_add_test(string)
esl_add_test(endian)
esl_add_test(map_utils)
esl_add_test(linked_list)
esl_add_test(lazy)
esl_add_test(base64)
esl_add_test(unicode)
esl_add_test(demangle)
esl_add_test(flex)
esl_add_test(flex_variant)
esl_add_test(json)
esl_add_test(yaml)
esl_add_test(shared_library)
esl_add_test(gm)

esl_add_shared(shared_library_tests_dl shared_library_tests)

