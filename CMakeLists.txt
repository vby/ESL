cmake_minimum_required(VERSION 3.11)

project(ESL CXX)

option(ESL_BUILD_TESTING "Enable ESL testing" ON)
option(ESL_ENABLE_YAML "Enable yaml" OFF)

file(GLOB_RECURSE sources "esl/*.cpp" "esl/*.hpp")
add_library(ESL ${sources})
target_compile_features(ESL PUBLIC cxx_std_17)
target_include_directories(ESL PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# libyaml
if(ESL_ENABLE_YAML)
    set(BUILD_TESTING OFF)
    add_subdirectory(thirdparty/libyaml)
    target_compile_definitions(ESL PUBLIC ESL_ENABLE_YAML)
    target_link_libraries(ESL PUBLIC yaml)
endif()

if(ESL_BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(DIRECTORY esl DESTINATION include)

