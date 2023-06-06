#ifndef PROTOBUFPARSER_UTILS_HPP
#define PROTOBUFPARSER_UTILS_HPP

#include "helpers.hpp"

// Вспомогательная функция для заполнения тестовых сообщений
/*!
       * \brief Заполняет сообщение данными.
       *
       * \tparam fieldType Тип данных, которыми требуется наполнить сообщение.
       *
       * \param fieldNumber Номер опционального поля, которое требуется заполнить.
       * \param field Данные, которыми заполнится сообщение.
       *
       * \return Готовое сообщение.
       */
template<typename fieldType>
WrapperMessage makeWrapperMessage(int fieldNumber, const fieldType& field) {
    WrapperMessage message;
    std::stringstream ss;
    ss << field;
    std::string fieldString = ss.str();

    switch (fieldNumber) {
        case 1:
            message.mutable_fast_response()->set_current_date_time(fieldString);
            break;
        case 2:
            message.mutable_slow_response()->set_connected_client_count(std::stoi(fieldString));
            break;
        case 3:
            break;
        case 4:
            message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(std::stoi(fieldString));
            break;
        default:
            std::cerr << "makeWrapperMessage: Invalid field number." << std::endl;
            break;
    }

    return message;
}

#endif //PROTOBUFPARSER_UTILS_HPP
