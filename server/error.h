//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <tuple>

//------------------------------------------------------------------------------

using namespace std::string_literals;

//------------------------------------------------------------------------------

struct [[nodiscard]] Error {

//------------------------------------------------------------------------------

    enum error {
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

    static auto strerror(Error::error error)
    {
        switch (error)
        {
            case no_error:
                return "Success"s;
            case invalid_channel_range:
                return "Invalid channel range"s;
            case invalid_channel_state:
                return "Invalid channel state"s;
            case invalid_channel_number:
                return "Invalid channel number"s;
            case invalid_argument_count:
                return "Invalid request arguments count"s;
            case invalid_argument:
                return "Invalid command argument"s;
            case invalid_command:
                return "Invalid command"s;
            case server_socket_failed:
                return "Server: socket() failed"s;
            case server_bind_failed:
                return "Server: bind() failed"s;
            case server_listen_failed:
                return "Server: listen() failed"s;
            case server_sigaction_failed:
                return "Server: sigaction() failed"s;
            case server_accept_failed:
                return "Server: accept() failed"s;
            case client_socket_failed:
                return "Client: socket() failed"s;
            case client_bind_failed:
                return "Client: bind() failed"s;
            case client_connect_failed:
                return "Client: connect() failed"s;
            case client_recv_failed:
                return "Client: recv() failed"s;
            case client_send_failed:
                return "Server: send() failed"s;
        }
        return "Undefined error"s;
    }

//------------------------------------------------------------------------------

    static auto success(Error::error error)
    {
        return error == no_error;
    }

//------------------------------------------------------------------------------


    static auto strerror(int error)
    {
        return strerror(static_cast<Error::error>(error));
    }

//------------------------------------------------------------------------------

};  //End of class Error

//------------------------------------------------------------------------------

using result_t = std::tuple<Error::error, std::string>;

//------------------------------------------------------------------------------