add_library(LRU_context_lib LRU_context.cc)
target_include_directories(LRU_context_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)

add_executable(LRU_tests LRU_tests.cc)
target_link_libraries(LRU_tests PRIVATE LRU_context_lib GTest::gtest_main)

gtest_discover_tests(LRU_tests)

add_executable(prog LRU.cc)
target_link_libraries(prog PRIVATE LRU_context_lib)

