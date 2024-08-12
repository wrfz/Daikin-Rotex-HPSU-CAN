#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>
#include "dummys.h"
#include "utils.h"
#include "request.h"

using TV8 = std::vector<uint8_t>;
using TVS = std::vector<std::string>;

using ::testing::AtLeast;

class MockESP32Can : public esphome::esp32_can::ESP32Can {
public:
    MOCK_METHOD(void, send_data, (uint32_t, bool, const std::vector<uint8_t>&), ());
};

TEST(TestRequest, send) {
    TRequest request = {
        "Status Kessel",
        {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
        {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
        [](auto const& data) -> std::string { return ""; }
    };

    MockESP32Can can;

    EXPECT_CALL(can, send_data(0x680, false, TV8({0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00})))
        .Times(1);

    g_millis = 1234;

    EXPECT_FALSE(request.inProgress());
    request.sendGet(&can);
    EXPECT_TRUE(request.inProgress());
}

TEST(TestRequest, isMatch) {
    {
        TRequest request = {
            "Status Kessel",
            {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
            {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
            [](auto const& data) -> std::string { return ""; }
        };

        EXPECT_TRUE(request.isMatch(0x180, TV8({0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00})));
        EXPECT_TRUE(request.isMatch(0x180, TV8({0x15, 0x34, 0xFA, 0x0A, 0x8C, 0x18, 0xAB})));

        EXPECT_FALSE(request.isMatch(0x181, TV8({0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00})));
        EXPECT_FALSE(request.isMatch(0x180, TV8({0x00, 0x00, 0xFB, 0x0A, 0x8C, 0x00, 0x00})));
        EXPECT_FALSE(request.isMatch(0x180, TV8({0x00, 0x00, 0xFA, 0x0B, 0x8C, 0x00, 0x00})));
        EXPECT_FALSE(request.isMatch(0x180, TV8({0x00, 0x00, 0xFA, 0x0A, 0x8D, 0x00, 0x00})));
    }

    {
        TRequest request = {
            "Status Kessel",
            {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
            0x300,
            {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
            [](auto const& data) -> std::string { return ""; }
        };

        EXPECT_FALSE(request.isMatch(0x180, TV8({0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00})));
        EXPECT_TRUE(request.isMatch(0x300, TV8({0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00})));
    }
}

TEST(TestRequest, handle) {
    {
        uint8_t v0 = 0;
        uint8_t v1 = 0;
        uint8_t v5 = 0;
        uint8_t v6 = 0;
        TRequest request = {
            "Status Kessel",
            {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
            {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
            [&v0, &v1, &v5, &v6](auto const& data) -> std::string {
                v0 = data[0];
                v1 = data[1];
                v5 = data[5];
                v6 = data[6];
                return "";
            }
        };

        EXPECT_EQ(request.getLastUpdate(), 0);
        EXPECT_TRUE(request.handle(0x180, TV8({0x54, 0xD8, 0xFA, 0x0A, 0x8C, 0x13, 0x18}), 12345));
        EXPECT_EQ(v0, 0x54);
        EXPECT_EQ(v1, 0xD8);
        EXPECT_EQ(v5, 0x13);
        EXPECT_EQ(v6, 0x18);
        EXPECT_EQ(request.getLastUpdate(), 12345);

        EXPECT_FALSE(request.handle(0x181, TV8({0x54, 0xD8, 0xFA, 0x0A, 0x8C, 0x13, 0x18}), 2345));
        EXPECT_EQ(request.getLastUpdate(), 12345);
        EXPECT_FALSE(request.handle(0x180, TV8({0x54, 0xD8, 0x0A, 0x0A, 0x8C, 0x13, 0x18}), 345));
        EXPECT_EQ(request.getLastUpdate(), 12345);
        EXPECT_FALSE(request.handle(0x180, TV8({0x54, 0xD8, 0xFA, 0x1A, 0x8C, 0x13, 0x18}), 4567));
        EXPECT_EQ(request.getLastUpdate(), 12345);
        EXPECT_FALSE(request.handle(0x180, TV8({0x54, 0xD8, 0xFA, 0x0A, 0x9C, 0x13, 0x18}), 13245));
        EXPECT_EQ(request.getLastUpdate(), 12345);

        EXPECT_TRUE(request.handle(0x180, TV8({0x54, 0xD8, 0xFA, 0x0A, 0x8C, 0x13, 0x18}), 17245));
        EXPECT_EQ(request.getLastUpdate(), 17245);
    }
}

TEST(TestRequest, inProgress) {
    TRequest request = {
        "Status Kessel",
        {0x31, 0x00, 0xFA, 0x0A, 0x8C, 0x00, 0x00},
        0x200,
        {  DC,   DC, 0xFA, 0x0A, 0x8C,   DC,   DC},
        [](auto const& data) -> std::string {return ""; }
    };

    esphome::esp32_can::ESP32Can can;

    EXPECT_FALSE(request.inProgress());
    g_millis = 1000;
    request.sendGet(&can);
    EXPECT_TRUE(request.inProgress());

    EXPECT_TRUE(request.handle(0x200, TV8({0x54, 0xD8, 0xFA, 0x0A, 0x8C, 0x13, 0x18}), 2000));
    EXPECT_FALSE(request.inProgress());

    g_millis = 3000;
    request.sendGet(&can);
    EXPECT_TRUE(request.inProgress());
    g_millis = 4000;
    EXPECT_TRUE(request.inProgress());
    g_millis = 5000;
    EXPECT_TRUE(request.inProgress());
    g_millis = 6000;
    EXPECT_FALSE(request.inProgress());
}
