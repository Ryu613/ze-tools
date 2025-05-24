cmake_minimum_required(VERSION 3.27)

project(ZE_TOOLS_ECS)

file(GLOB_RECURSE ze_tools_ecs_src CONFIGURE_DEPENDS include/*.hpp src/*.cpp)

add_library(ze_tools_ecs STATIC ${ze_tools_ecs_src})

target_include_directories(ze_tools_ecs PUBLIC include)

add_executable(test_ecs test/test_ecs.cpp)

target_link_libraries(test_ecs PRIVATE ze_tools_ecs)

