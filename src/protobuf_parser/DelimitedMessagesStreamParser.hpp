#ifndef CPP_PROTOBUF_DELIMITEDMESSAGESSTREAMPARSER_HPP
#define CPP_PROTOBUF_DELIMITEDMESSAGESSTREAMPARSER_HPP

#include <list>
#include "helpers.hpp"

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif

using namespace TestTask::Messages;

template<typename MessageType>
using PointerToConstValue = std::shared_ptr<const MessageType>;

template<typename MessageType>
class DelimitedMessagesStreamParser
{
public:
    using PointerToConstValue = std::shared_ptr<const MessageType>;

    std::list<PointerToConstValue> parse(const std::string& data) {
        std::list<PointerToConstValue> parsedMessages;
        m_buffer.insert(m_buffer.end(), data.begin(), data.end());
        size_t bytesProcessed = 0; // количество байт, затраченных на расшифровку сообщения(й)

        while (bytesProcessed < m_buffer.size()) {
            size_t bytesConsumed = 0;
            auto message = parseDelimited<MessageType>(&m_buffer[bytesProcessed],
                                                       m_buffer.size() - bytesProcessed,
                                                       &bytesConsumed);
            if (!message) { // Если сообщение не удалось разобрать, прерываем работу парсера,
                            // при этом в буфере остаются неполные данные
                            // (возможно недостающие байты придут в следующем потоке)
                break;
            }
            // Если очередное сообщение удалось разобрать, добавляем его в список разобранных сообщений
            // и обновляем количество байт, затраченных на расшифровку сообщения
            parsedMessages.push_back(message);
            bytesProcessed += bytesConsumed;
        }

        if (bytesProcessed > 0) {
            // Если одно или более сообщений успешно разобрано, удаляем их из буфера
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesProcessed);
        }
        return parsedMessages;
    }

private:
    std::vector<char> m_buffer;
};

#endif //CPP_PROTOBUF_DELIMITEDMESSAGESSTREAMPARSER_HPP
