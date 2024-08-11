#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "dummys.h"
#include "utils.h"

using TV8 = std::vector<uint8_t>;
using TVS = std::vector<std::string>;

TEST(TestUtils, to_hex_uint8) {
    EXPECT_EQ("0x00", Utils::to_hex(0x00u));
    EXPECT_EQ("0x3C", Utils::to_hex(0x3Cu));
    EXPECT_EQ("0xFF", Utils::to_hex(0xFFu));
}

TEST(TestUtils, to_hex_container) {
    EXPECT_EQ("0x00", Utils::to_hex(TV8({{0x00}})));
    EXPECT_EQ("0x3C", Utils::to_hex(TV8({{0x3C}})));
    EXPECT_EQ("0xFF", Utils::to_hex(TV8({{0xFF}})));

    EXPECT_EQ("0x00 0xA8 0xFE", Utils::to_hex(TV8({{0x00, 0xA8, 0xFE}})));
}

TEST(TestUtils, is_number) {
    EXPECT_TRUE(Utils::is_number("0"));
    EXPECT_TRUE(Utils::is_number("5"));
    EXPECT_TRUE(Utils::is_number("9"));
    EXPECT_TRUE(Utils::is_number("12"));
    EXPECT_TRUE(Utils::is_number("5534"));

    EXPECT_FALSE(Utils::is_number("0u"));
    EXPECT_FALSE(Utils::is_number(" 0"));
    EXPECT_FALSE(Utils::is_number("0 "));
    EXPECT_FALSE(Utils::is_number("123a64"));
    EXPECT_FALSE(Utils::is_number("0x123"));
}

TEST(TestUtils, split) {
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(Utils::split("ab")));
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(Utils::split("ab|")));
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(Utils::split("|ab")));
    EXPECT_THAT(TVS({{"ab"}, {"cd"}}), ::testing::ElementsAreArray(Utils::split("ab|cd")));
    EXPECT_THAT(TVS({{"ab"}, {"cd"}, {"ef"}}), ::testing::ElementsAreArray(Utils::split("ab|cd|ef")));
}

TEST(TestUtils, str_format) {
    EXPECT_EQ("ab|cd", Utils::str_format("%s|%s", "ab", "cd"));
    EXPECT_EQ("1|2", Utils::str_format("%d|%d", 1, 2));
    EXPECT_EQ("1.230000|4.560000", Utils::str_format("%f|%f", 1.23, 4.56));
}

TEST(TestUtils, bidi_map) {

    BidiMap<uint8_t, std::string> map = {
        {0x12, "abc"},
        {0x13, "def"}
    };

    {
        auto const iter = map.findByKey(0x12);
        ASSERT_EQ(iter->first, 0x12);
        ASSERT_EQ(iter->second, "abc");
    }
    {
        auto const iter = map.findByKey(0x13);
        ASSERT_EQ(iter->first, 0x13);
        ASSERT_EQ(iter->second, "def");
    }
    {
        auto const iter = map.findByKey(0x34);
        ASSERT_EQ(iter, map.end());
    }

    {
        auto const iter = map.findByValue("abc");
        ASSERT_EQ(iter->first, 0x12);
        ASSERT_EQ(iter->second, "abc");
    }
    {
        auto const iter = map.findByValue("def");
        ASSERT_EQ(iter->first, 0x13);
        ASSERT_EQ(iter->second, "def");
    }
    {
        auto const iter = map.findByValue("xyz");
        ASSERT_EQ(iter, map.end());
    }

    ASSERT_EQ(map.getKey("abc"), 0x12);
    ASSERT_EQ(map.getKey("def"), 0x13);
    ASSERT_EQ(map.getValue(0x12), "abc");
    ASSERT_EQ(map.getValue(0x13), "def");

    EXPECT_THROW(map.getKey("xyz"), std::runtime_error);
    EXPECT_THROW(map.getValue(0x34), std::runtime_error);
}
