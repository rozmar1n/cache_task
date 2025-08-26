cmake_minimum_required(VERSION 3.15)

project(LRU)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(LRU)

add_executable(prog main.cc)
target_link_libraries(prog PRIVATE LRU_context_lib)
