//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <tuple>
#include <map>

//------------------------------------------------------------------------------

using namespace std::string_literals;

//------------------------------------------------------------------------------

#define ERROR_POS 0
#define VALUE_POS 1

//------------------------------------------------------------------------------

struct [[nodiscard]] Error {

//------------------------------------------------------------------------------

    enum class Type {
        no_error,
        invalid_channel_range,
        invalid_channel_state,
        invalid_channel_number,
        invalid_argument_count,
        invalid_argument,
        invalid_command,
        server_socket_failed,
        server_bind_failed,
        server_listen_failed,
        server_sigaction_failed,
        server_accept_failed,
        client_socket_failed,
        client_bind_failed,
        client_connect_failed,
        client_recv_failed,
        client_send_failed
    };

//------------------------------------------------------------------------------

    static auto strerror(Error::Type error)
    {
        return error_strings_.find(error)->second;
    }

//------------------------------------------------------------------------------

    static auto strerror(int error)
    {
        return strerror(static_cast<Error::Type>(error));
    }

//------------------------------------------------------------------------------

    static auto strerror(const std::tuple <Error::Type, std::string> &error)
    {
        return strerror(std::get<ERROR_POS>(error));
    }

//------------------------------------------------------------------------------

    static auto success(Error::Type error)
    {
        return error == Type::no_error;
    }

//------------------------------------------------------------------------------

    static auto success(const std::tuple <Error::Type, std::string> &error)
    {
        return success(std::get<ERROR_POS>(error));
    }

//------------------------------------------------------------------------------

    static auto value(const std::tuple <Error::Type, std::string> &value)
    {
        return std::get<VALUE_POS>(value);
    }

//------------------------------------------------------------------------------

private:

    static inline const std::map <Error::Type, std::string> error_strings_ = {
            {Type::no_error,                "Success"},
            {Type::invalid_channel_range,   "Invalid channel range"},
            {Type::invalid_channel_state,   "Invalid channel state"},
            {Type::invalid_channel_number,  "Invalid channel number"},
            {Type::invalid_argument_count,  "Invalid request arguments count"},
            {Type::invalid_argument,        "Invalid command argument"},
            {Type::invalid_command,         "Invalid command"},
            {Type::server_socket_failed,    "Server: socket() failed"},
            {Type::server_bind_failed,      "Server: bind() failed"},
            {Type::server_listen_failed,    "Server: listen() failed"},
            {Type::server_sigaction_failed, "Server: sigaction() failed"},
            {Type::server_accept_failed,    "Server: accept() failed"},
            {Type::client_socket_failed,    "Client: socket() failed"},
            {Type::client_bind_failed,      "Client: bind() failed"},
            {Type::client_connect_failed,   "Client: connect() failed"},
            {Type::client_recv_failed,      "Client: recv() failed"},
            {Type::client_send_failed,      "Server: send() failed"}};

//------------------------------------------------------------------------------

};  //End of class Error

//------------------------------------------------------------------------------

using result_t = std::tuple<Error::Type, std::string>;

//------------------------------------------------------------------------------
