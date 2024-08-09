#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "utils.h"

using TV8 = std::vector<uint8_t>;
using TVS = std::vector<std::string>;

TEST(HPSUTest, to_hex_uint8) {
    EXPECT_EQ("0x00", to_hex(0x00u));
    EXPECT_EQ("0x3C", to_hex(0x3Cu));
    EXPECT_EQ("0xFF", to_hex(0xFFu));
}

TEST(HPSUTest, to_hex_container) {
    EXPECT_EQ("0x00", to_hex(TV8({{0x00}})));
    EXPECT_EQ("0x3C", to_hex(TV8({{0x3C}})));
    EXPECT_EQ("0xFF", to_hex(TV8({{0xFF}})));

    EXPECT_EQ("0x00 0xA8 0xFE", to_hex(TV8({{0x00, 0xA8, 0xFE}})));
}

TEST(HPSUTest, is_number) {
    EXPECT_TRUE(is_number("0"));
    EXPECT_TRUE(is_number("5"));
    EXPECT_TRUE(is_number("9"));
    EXPECT_TRUE(is_number("12"));
    EXPECT_TRUE(is_number("5534"));

    EXPECT_FALSE(is_number("0u"));
    EXPECT_FALSE(is_number(" 0"));
    EXPECT_FALSE(is_number("0 "));
    EXPECT_FALSE(is_number("123a64"));
    EXPECT_FALSE(is_number("0x123"));
}

TEST(HPSUTest, split) {
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(split("ab")));
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(split("ab|")));
    EXPECT_THAT(TVS({{"ab"}}), ::testing::ElementsAreArray(split("|ab")));
    EXPECT_THAT(TVS({{"ab"}, {"cd"}}), ::testing::ElementsAreArray(split("ab|cd")));
    EXPECT_THAT(TVS({{"ab"}, {"cd"}, {"ef"}}), ::testing::ElementsAreArray(split("ab|cd|ef")));
}

TEST(HPSUTest, str_format) {
    EXPECT_EQ("ab|cd", str_format("%s|%s", "ab", "cd"));
    EXPECT_EQ("1|2", str_format("%d|%d", 1, 2));
    EXPECT_EQ("1.230000|4.560000", str_format("%f|%f", 1.23, 4.56));
}
