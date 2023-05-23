#include "DelimitedMessagesStreamParser.hpp"
#include <gtest/gtest.h>

// Разбираем поток байт из трёх сообщений WrapperMessage с разными опциональными полями.
// Разбор идёт по одному байту за раз.
TEST(DelimitedMessagesStreamParserTest, byteByByteTest) {
    std::string iso_time("20230515T143233.123");
    int client_count = 314;
    int time_to_sleep = 2718;

    WrapperMessage wrapperMessage = makeWrapperMessage(1, iso_time);
    auto res = serializeDelimited(wrapperMessage);
    std::string msg(res->begin(), res->end());

    wrapperMessage = makeWrapperMessage(2, client_count);
    res = serializeDelimited(wrapperMessage);
    msg += std::string(res->begin(), res->end());

    wrapperMessage = makeWrapperMessage(4, time_to_sleep);
    res = serializeDelimited(wrapperMessage);
    msg += std::string(res->begin(), res->end());

    Data messages(msg.begin(), msg.end());
    using Parser = DelimitedMessagesStreamParser<WrapperMessage>;
    Parser parser;
    std::list<PointerToConstValue<WrapperMessage>> storedMessages;

    for (const char byte : messages)
    { // Разбор идёт по одному байту за раз.
        const auto& parsedMessages = parser.parse(std::string(1, byte));
        for (const auto& value : parsedMessages)
        { // сохраняем разобранные сообщения (если они есть) для проверки
            storedMessages.push_back(value);
        }
    }

    for (const auto& message : storedMessages) {
        if (message->has_fast_response()) {
            EXPECT_EQ(message->fast_response().current_date_time(), iso_time);
            EXPECT_FALSE(message->has_slow_response());
            EXPECT_FALSE(message->has_request_for_fast_response());
            EXPECT_FALSE(message->has_request_for_slow_response());
        } else if (message->has_slow_response()) {
            EXPECT_EQ(message->slow_response().connected_client_count(), client_count);
            EXPECT_FALSE(message->has_fast_response());
            EXPECT_FALSE(message->has_request_for_fast_response());
            EXPECT_FALSE(message->has_request_for_slow_response());
        } else if (message->has_request_for_fast_response()) {
            EXPECT_FALSE(message->has_fast_response());
            EXPECT_FALSE(message->has_slow_response());
            EXPECT_FALSE(message->has_request_for_slow_response());
        } else if (message->has_request_for_slow_response()) {
            EXPECT_EQ(message->request_for_slow_response().time_in_seconds_to_sleep(), time_to_sleep);
            EXPECT_FALSE(message->has_fast_response());
            EXPECT_FALSE(message->has_slow_response());
            EXPECT_FALSE(message->has_request_for_fast_response());
        }
    }

}
