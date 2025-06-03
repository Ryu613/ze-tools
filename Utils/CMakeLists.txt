cmake_minimum_required(VERSION 3.27)

project(ZE_TOOLS_UTILS)

file(GLOB_RECURSE ze_tools_utils_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp src/*.hpp)

add_library(ze_tools_utils STATIC ${ze_tools_utils_src})

target_include_directories(ze_tools_utils PUBLIC include)

add_executable(test_filesystem "test/test_filesystem.cpp")

target_link_libraries(test_filesystem PRIVATE ze_tools_utils)

