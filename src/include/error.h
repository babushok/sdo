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

    enum class type {
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

    static auto strerror(Error::type error)
    {
        return error_strings_.find(error)->second;
    }

//------------------------------------------------------------------------------

    static auto strerror(int error)
    {
        return strerror(static_cast<Error::type>(error));
    }

//------------------------------------------------------------------------------

    static auto strerror(const std::tuple <Error::type, std::string> &error)
    {
        return strerror(std::get<ERROR_POS>(error));
    }

//------------------------------------------------------------------------------

    static auto success(Error::type error)
    {
        return error == type::no_error;
    }

//------------------------------------------------------------------------------

    static auto success(const std::tuple <Error::type, std::string> &error)
    {
        return success(std::get<ERROR_POS>(error));
    }

//------------------------------------------------------------------------------

    static auto value(const std::tuple <Error::type, std::string> &value)
    {
        return std::get<VALUE_POS>(value);
    }

//------------------------------------------------------------------------------

private:

    static inline const std::map <Error::type, std::string> error_strings_ = {
            {type::no_error,                "Success"},
            {type::invalid_channel_range,   "Invalid channel range"},
            {type::invalid_channel_state,   "Invalid channel state"},
            {type::invalid_channel_number,  "Invalid channel number"},
            {type::invalid_argument_count,  "Invalid request arguments count"},
            {type::invalid_argument,        "Invalid command argument"},
            {type::invalid_command,         "Invalid command"},
            {type::server_socket_failed,    "Server: socket() failed"},
            {type::server_bind_failed,      "Server: bind() failed"},
            {type::server_listen_failed,    "Server: listen() failed"},
            {type::server_sigaction_failed, "Server: sigaction() failed"},
            {type::server_accept_failed,    "Server: accept() failed"},
            {type::client_socket_failed,    "Client: socket() failed"},
            {type::client_bind_failed,      "Client: bind() failed"},
            {type::client_connect_failed,   "Client: connect() failed"},
            {type::client_recv_failed,      "Client: recv() failed"},
            {type::client_send_failed,      "Server: send() failed"}};

//------------------------------------------------------------------------------

};  //End of class Error

//------------------------------------------------------------------------------

using result_t = std::tuple<Error::type, std::string>;

//------------------------------------------------------------------------------
