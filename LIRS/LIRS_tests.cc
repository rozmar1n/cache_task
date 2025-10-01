#include <gtest/gtest.h>
#include "LIRS.hpp"
#include <vector>

struct page_t {
    int id;
};

page_t slow_get_page(int id) {
    page_t retpg;
    retpg.id = id;
    return retpg;
}

int run_cache(size_t ch_size, int N_pushes, std::vector<int> pages) {
    LIRS::cache_t<page_t> ch{ch_size};
    int counter = 0;
    for(int i = 0; i < N_pushes; i++) {
        if(ch.lookup_update(pages[i], slow_get_page)) {
            counter++;
        }
    }
    return counter;
}

TEST(LIRSTest, ElevenHitsCountedByMe) {
    size_t ch_size = 5;
    std::vector<int> pages = {1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1};
    EXPECT_EQ(run_cache(ch_size, pages.size(), pages), 4);
}

TEST(LIRSTest, RepeatedAccess) {
    size_t ch_size = 3;
    std::vector<int> pages = {1, 2, 3, 1, 2, 3, 1, 2, 3};
    EXPECT_EQ(run_cache(ch_size, pages.size(), pages), 6);
}

TEST(LIRSTest, EdgeCaseSize2) {
    size_t ch_size = 2;
    std::vector<int> pages = {1, 2, 3, 1, 2, 3};
    int hits = run_cache(ch_size, pages.size(), pages);
    EXPECT_EQ(hits, 1);
}

TEST(LIRSTest, LargeCache) {
    size_t ch_size = 10000000;
    int res = ch_size;
    std::vector<int> pages;
    for(int i = 0; i < ch_size; i++) {
        pages.push_back(i);
    }
    for(int i = 0; i < ch_size; i++) {
        pages.push_back(i);
    }
    EXPECT_EQ(run_cache(ch_size, pages.size(), pages), res);
}

TEST(LIRSTest, HIRNonResidentRecovery) {
    size_t ch_size = 3;
    std::vector<int> pages = {1, 2, 3, 4, 5, 4};
    int hits = run_cache(ch_size, pages.size(), pages);
    EXPECT_EQ(hits, 0);  
}

TEST(LIRSTest, TwoLirResidents) {
    size_t ch_size = 3;
    std::vector<int> pages = {1, 2, 3, 4, 1, 2, 3, 4};
    EXPECT_EQ(run_cache(ch_size, pages.size(), pages), 2);
}


