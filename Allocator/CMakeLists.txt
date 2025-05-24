cmake_minimum_required(VERSION 3.27)

project(ZE_TOOLS_ALLOCATOR)

file(GLOB_RECURSE ze_tools_allocator_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp)

add_library(ze_tools_allocator STATIC ${ze_tools_allocator_src})

target_include_directories(ze_tools_allocator PUBLIC include)

