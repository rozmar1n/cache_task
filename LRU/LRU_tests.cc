#include <gtest/gtest.h>
#include "LRU_context.hpp"

TEST(LRUTest, NoHitsOnUniqueKeys) {
    LRU::cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 3};
    EXPECT_EQ(ctx.run(), 0);
}

TEST(LRUTest, HitOnRepeat) {
    LRU::cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 1};
    EXPECT_EQ(ctx.run(), 1);
}

TEST(LRUTest, HitOnRepeatMany) {
    LRU::cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 1, 2, 1, 2};
    EXPECT_EQ(ctx.run(), 4);
}

TEST(LRUTest, VoidCache) {
    LRU::cache_test_context ctx;
    ctx.cz = 0;
    ctx.data = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(ctx.run(), 0);
}

TEST(LRUTest, SingleCellCache) {
    LRU::cache_test_context ctx;
    ctx.cz = 1;
    ctx.data = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(ctx.run(), 1);
}

TEST(LRUTest, CacheBiggerThanData) {
    LRU::cache_test_context ctx;
    ctx.cz = 10;
    ctx.data = {1, 2, 3, 1, 2, 3};
    EXPECT_EQ(ctx.run(), 3);
}
