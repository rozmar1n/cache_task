
#include <gtest/gtest.h>
#include "ideal.hpp"
#include <vector>

struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    page_t retpg;
    retpg.id = id;
    return retpg;
}


TEST(idealTests, test_1) {
    size_t ch_size = 2;
    std::vector<int> pages = {95, 36, 32, 29, 18};
    int expected = 0;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_2) {
    size_t ch_size = 2;
    std::vector<int> pages = {76, 55, 5, 4, 12, 28, 30};
    int expected = 0;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_3) {
    size_t ch_size = 9;
    std::vector<int> pages = {72, 26, 92, 84, 90};
    int expected = 0;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_4) {
    size_t ch_size = 9;
    std::vector<int> pages = {29, 58, 76, 36, 1, 98, 21, 90, 55, 44, 36, 20, 28, 98, 44, 14, 12, 49};
    int expected = 3;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_5) {
    size_t ch_size = 2;
    std::vector<int> pages = {45, 78, 34, 6, 94, 59, 69, 16, 49, 11, 71, 38, 81, 80, 47, 74};
    int expected = 0;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_6) {
    size_t ch_size = 4;
    std::vector<int> pages = {6, 85, 30, 99, 38, 11, 30};
    int expected = 1;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_7) {
    size_t ch_size = 2;
    std::vector<int> pages = {36, 59, 82, 47, 21, 48, 46, 27, 86, 35, 90, 88, 83, 10, 78, 82, 22};
    int expected = 1;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_8) {
    size_t ch_size = 9;
    std::vector<int> pages = {21, 60, 49, 35, 82, 89, 72, 29, 88, 42, 99, 100};
    int expected = 0;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_9) {
    size_t ch_size = 1;
    std::vector<int> pages = {5, 41, 52, 35, 9, 28, 73, 92, 41, 28, 84, 64};
    int expected = 1;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(idealTests, test_10) {
    size_t ch_size = 7;
    std::vector<int> pages = {19, 34, 18, 32, 96, 72, 69, 34, 96, 75, 55, 75, 52, 47, 29, 18, 66, 64, 12};
    int expected = 4;

    ideal::cache_t<page_t> ch{ch_size, pages};
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}
