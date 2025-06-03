cmake_minimum_required(VERSION 3.27)

project(ZE_TOOLS)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(Allocator)
add_subdirectory(Collection)
add_subdirectory(ECS)
add_subdirectory(Utils)