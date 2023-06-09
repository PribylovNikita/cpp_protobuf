#ifndef CPP_PROTOBUF_HELPERS_HPP
#define CPP_PROTOBUF_HELPERS_HPP

#include <vector>
#include <memory>
#include <sstream>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "messages.pb.h"

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif

using namespace TestTask::Messages;

using Data = std::vector<char>;
using PointerToData = std::shared_ptr<Data>;
using PointerToConstData = std::shared_ptr<const Data>;

// Вспомогательная функция для сериализации сообщений в поток байт
template <typename Message>
PointerToConstData serializeDelimited(const Message& msg)
{
    const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
    const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

    const PointerToData& result = std::make_shared<Data>(headerSize + messageSize);
    google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());

    google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
    msg.SerializeWithCachedSizesToArray(buffer + headerSize);

    return result;
}

/*!
       * \brief Расшифровывает сообщение, предваренное длиной из массива байтов.
       *
       * \tparam Message Тип сообщения, для работы с которым предназначена данная функция.
       *
       * \param data Указатель на буфер данных.
       * \param size Размер буфера данных.
       * \param bytesConsumed Количество байт, которое потребовалось для расшифровки сообщения в случае успеха.
       *
       * \return Умный указатель на сообщение. Если удалось расшифровать сообщение, то он не пустой.
       */
template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed = nullptr) {
    google::protobuf::io::ArrayInputStream input(data, size);
    google::protobuf::io::CodedInputStream codedInput(&input);

    // Считываем размер сообщения
    uint32_t messageSize;
    if (!codedInput.ReadVarint32(&messageSize)) {
        //std::cerr << "Failed to read message size." << std::endl;
        return nullptr;
    }

    // Проверяем, что заявленный размер данных не меньше чем считанный размер сообщения
    if (size < messageSize + codedInput.CurrentPosition()) {
        //std::cerr << "Incomplete message." << std::endl;
        return nullptr;
    }

    // Ограничим количество байт, которое будем разбирать, длиной сообщения
    google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(messageSize);

    // Разбираем сообщение из потока
    std::shared_ptr<Message> message = std::make_shared<Message>();
    if (!message->ParseFromCodedStream(&codedInput)) {
        //std::cerr << "Failed to parse WrapperMessage" << std::endl;
        return nullptr;
    }

    // Снимаем ограничения
    codedInput.PopLimit(limit);

    // Если сообщение было успешно разобрано, обновляем количество байт, затраченных на расшифровку
    if (bytesConsumed != nullptr) {
        *bytesConsumed = codedInput.CurrentPosition();
    }

    return message;
}

#endif //CPP_PROTOBUF_HELPERS_HPP
