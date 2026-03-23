#include <gtest/gtest.h>
#include "cpp_hw_2.h"
#include <vector>

TEST(ApplyFunctionTest, SingleThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 1);

    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(data, expected);
}

TEST(ApplyFunctionTest, MultiThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 3);
    
    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(data, expected);
}

TEST(ApplyFunctionTest, EQDataSizeThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 5);
    
    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(data, expected);
}

TEST(ApplyFunctionTest, MoreThenDataSizeThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 10);
    
    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(data, expected);
}

TEST(ApplyFunctionTest, ZeroThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 0);
    
    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(data, expected);
}

TEST(ApplyFunctionTest, EmptyData) {
    std::vector<int> data;

    ApplyFunction<int>(data, [](int& x) {
        x += 1;
    }, 4);

    EXPECT_TRUE(data.empty());
}

TEST(ApplyFunctionTest, ElementOneTouch) {
    std::vector<int> data(10'000'000, 1);

    ApplyFunction<int>(data, [](int& x) {
        x *= 2;
    }, 4);

    for (int x : data) {
        EXPECT_EQ(x, 2);
    }
}
