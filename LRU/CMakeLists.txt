add_library(LRU_context_lib LRU_context.cc)
target_include_directories(LRU_context_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_executable(LRU_tests LRU_tests.cc)
target_link_libraries(LRU_tests PRIVATE LRU_context_lib GTest::gtest_main)

gtest_discover_tests(LRU_tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cli)
add_executable(LRU.out LRU.cc)
target_link_libraries(LRU.out PRIVATE LRU_context_lib)

