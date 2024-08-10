#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
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
