include(GoogleTest)

add_executable(ideal_tests ideal_tests.cc)
target_link_libraries(ideal_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(ideal_tests)

add_executable(ideal_big_tests ideal_big_tests.cc)
target_link_libraries(ideal_big_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(ideal_big_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
    DISCOVERY_MODE PRE_TEST
)

add_executable(ideal.out ideal.cc)

