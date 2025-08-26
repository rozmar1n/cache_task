#include <gtest/gtest.h>
#include "LRU_context.hpp"

TEST(LRUTest, NoHitsOnUniqueKeys) {
    cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 3};
    EXPECT_EQ(ctx.run(), 0);
}

TEST(LRUTest, HitOnRepeat) {
    cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 1};
    EXPECT_EQ(ctx.run(), 1);
}

TEST(LRUTest, HitOnRepeatMany) {
    cache_test_context ctx;
    ctx.cz = 2;
    ctx.data = {1, 2, 1, 2, 1, 2};
    EXPECT_EQ(ctx.run(), 4);
}
