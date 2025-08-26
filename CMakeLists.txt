cmake_minimum_required(VERSION 3.15)

project(LRU)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library (LRU_context_lib LRU_context.cc)
target_include_directories(LRU_context_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(LRU main.cc)
target_link_libraries(LRU PRIVATE LRU_context_lib)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(LRU_test tests.cc)
target_link_libraries(LRU_test PRIVATE LRU_context_lib GTest::gtest_main)

gtest_discover_tests(LRU_test)
