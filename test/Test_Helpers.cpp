#include "helpers.hpp"
#include <gtest/gtest.h>

TEST(parseDelimitedTest, FastResponseTest) {
    std::string iso_time("20230515T143233.123");
    WrapperMessage wrapperMessage = makeWrapperMessage(1, iso_time);
    auto res = serializeDelimited(wrapperMessage);
    std::string msg(res->begin(), res->end());
    auto message = parseDelimited<WrapperMessage>(msg.c_str(), msg.length());
    EXPECT_EQ(message->fast_response().current_date_time(), iso_time);
    EXPECT_FALSE(message->has_slow_response());
    EXPECT_FALSE(message->has_request_for_fast_response());
    EXPECT_FALSE(message->has_request_for_slow_response());
}

TEST(parseDelimitedTest, SlowResponseTest) {
    int client_count = 314;
    WrapperMessage wrapperMessage = makeWrapperMessage(2, client_count);
    auto res = serializeDelimited(wrapperMessage);
    std::string msg = std::string(res->begin(), res->end());
    auto message = parseDelimited<WrapperMessage>(msg.c_str(), msg.length());
    EXPECT_EQ(message->slow_response().connected_client_count(), 314);
    EXPECT_FALSE(message->has_fast_response());
    EXPECT_FALSE(message->has_request_for_fast_response());
    EXPECT_FALSE(message->has_request_for_slow_response());
}

TEST(parseDelimitedTest, RequestForSlowResponseTest) {
    int time_to_sleep = 2718;
    WrapperMessage wrapperMessage = makeWrapperMessage(4, time_to_sleep);
    auto res = serializeDelimited(wrapperMessage);
    std::string msg(res->begin(), res->end());
    auto message = parseDelimited<WrapperMessage>(msg.c_str(), msg.length());
    EXPECT_EQ(message->request_for_slow_response().time_in_seconds_to_sleep(), 2718);
    EXPECT_FALSE(message->has_fast_response());
    EXPECT_FALSE(message->has_slow_response());
    EXPECT_FALSE(message->has_request_for_fast_response());
}
