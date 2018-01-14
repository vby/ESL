cmake_minimum_required(VERSION 2.8)

project(ESL_tests CXX)

include(ExternalProject)

ExternalProject_Add(
		catch
		PREFIX ${CMAKE_BINARY_DIR}/catch
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG v2.0.1
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external -DNO_SELFTEST=1 
		LOG_DOWNLOAD 1
		UPDATE_DISCONNECTED 1
	)

#add_subdirectory(${CMAKE_BINARY_DIR}/catch/src/catch ${CMAKE_BINARY_DIR}/catch/build)

function(add_esl_test name)
    add_executable(${name} ${name}.cpp)
	target_include_directories(${name} PUBLIC ${CMAKE_BINARY_DIR}/external/include)
    target_link_libraries(${name} ESL)
	add_dependencies(${name} catch)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_esl_test(span_tests)

