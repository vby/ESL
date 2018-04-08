cmake_minimum_required(VERSION 3.1)

project(ESL CXX)

option(ESL_BUILD_TESTING "Enable ESL testing" ON)

add_library(ESL INTERFACE)

target_include_directories(ESL INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
	)

target_compile_features(ESL INTERFACE cxx_std_17)

# libyaml
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/libyaml)
target_link_libraries(ESL INTERFACE yaml)

install(DIRECTORY esl DESTINATION include)

if(ESL_BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()

