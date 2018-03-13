cmake_minimum_required(VERSION 3.1)

project(ESL CXX)

include(CTest)

option(WITH_YAML "Enable yaml support" ON)

add_library(esl INTERFACE)

target_include_directories(esl INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
	)

target_compile_features(esl INTERFACE cxx_std_17)

if(WITH_YAML) 
	set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
	add_subdirectory(thirdparty/libyaml)
	target_link_libraries(esl INTERFACE yaml)
endif()

install(DIRECTORY esl DESTINATION include)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(ESL_MAIN_PROJECT ON)
else()
	set(ESL_MAIN_PROJECT OFF)
endif()

set(BUILD_TESTING ${ESL_MAIN_PROJECT} CACHE BOOL "" FORCE)
if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()

