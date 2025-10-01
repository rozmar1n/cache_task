include(GoogleTest)

add_executable(LIRS_tests LIRS_tests.cc)
target_link_libraries(LIRS_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(LIRS_tests)

add_executable(LIRS.out LIRS.cc)
