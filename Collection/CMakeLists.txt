cmake_minimum_required(VERSION 3.27)

project(ZE_TOOLS_COLLECTION)

file(GLOB_RECURSE ze_tools_collection_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp)

add_library(ze_tools_collection INTERFACE ${ze_tools_collection_src})

target_include_directories(ze_tools_collection INTERFACE include)

add_executable(test_sparse_set test/test_sparse_set.cpp)

target_link_libraries(test_sparse_set PRIVATE ze_tools_collection)

