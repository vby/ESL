cmake_minimum_required(VERSION 2.8)

project(ESL CXX)

add_library(ESL INTERFACE)

target_include_directories(ESL INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
	)

install(DIRECTORY esl DESTINATION include)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(ESL_MAIN_PROJECT ON)
endif()

option(ESL_TEST "Generate tests" ${ESL_MAIN_PROJECT})
if(ESL_TEST)
	enable_testing()
	add_subdirectory(tests)
endif()

