#include <gtest/gtest.h>
#include "ideal.hpp"
#include <vector>
#include <fstream>

struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    page_t retpg;
    retpg.id = id;
    return retpg;
}

void run_single_test(const std::string& basepath) {
    size_t ch_size = 0;
    std::vector<int> pages;
    int expected = 0;

    std::ifstream dat(basepath + ".dat");
    ASSERT_TRUE(dat.is_open());

    dat >> ch_size;
    size_t n_hits = 0;
    dat >> n_hits;
    pages.reserve(n_hits);

    int hit;
    for(size_t i = 0; i < n_hits; i++) {
        dat >> hit;
        pages.push_back(hit);
    }

    std::ifstream sol(basepath + ".sol");
    ASSERT_TRUE(sol.is_open());
    sol >> expected;

    ideal::cache_t<page_t> ch(ch_size, pages);
    EXPECT_EQ(ch.run_cache(slow_get_page), expected);
}

TEST(BigIdealTests, test_1) {run_single_test("ideal/big_tests/001"); }
TEST(BigIdealTests, test_2) {run_single_test("ideal/big_tests/002"); }
TEST(BigIdealTests, test_3) {run_single_test("ideal/big_tests/003"); }
TEST(BigIdealTests, test_4) {run_single_test("ideal/big_tests/004"); }
TEST(BigIdealTests, test_5) {run_single_test("ideal/big_tests/005"); }
TEST(BigIdealTests, test_6) {run_single_test("ideal/big_tests/006"); }
TEST(BigIdealTests, test_7) {run_single_test("ideal/big_tests/007"); }
TEST(BigIdealTests, test_8) {run_single_test("ideal/big_tests/008"); }
TEST(BigIdealTests, test_9) {run_single_test("ideal/big_tests/009"); }
TEST(BigIdealTests, test_10) {run_single_test("ideal/big_tests/010"); }
