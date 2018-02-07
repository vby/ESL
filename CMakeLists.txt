cmake_minimum_required(VERSION 3.1)

project(ESL CXX)

add_library(esl INTERFACE)

target_include_directories(esl INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
	)

target_compile_features(esl INTERFACE cxx_std_17)

install(DIRECTORY esl DESTINATION include)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(ESL_MAIN_PROJECT ON)
endif()

option(ESL_TEST "Generate tests" ${ESL_MAIN_PROJECT})
if(ESL_TEST)
	enable_testing()
	add_subdirectory(tests)
endif()

