//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <signal.h>

#include "log.h"
#include "common.h"

//------------------------------------------------------------------------------

class Client {

//------------------------------------------------------------------------------

public:

    enum class Message {
        client,
        command,
        end_command,
        send,
        send_fail,
        recv,
        recv_fail,
        error,
        send_color,
        recv_color,
        error_color,
        def_color,
        exit
    };

//------------------------------------------------------------------------------

    Client(std::string app_name) : app_name_{app_name} {}

//------------------------------------------------------------------------------

    auto init()
    {
        struct sockaddr_un client_sockaddr;
        struct sockaddr_un server_sockaddr;

        if (-1 == (client_sock_ = socket(AF_UNIX, SOCK_STREAM, 0))) return Error::type::client_socket_failed;

        memset(&client_sockaddr, 0, sizeof(client_sockaddr));
        client_sockaddr.sun_family = AF_UNIX;
        strcpy(client_sockaddr.sun_path, sun_path_.c_str());

        unlink(sun_path_.c_str());
        if (-1 == bind(client_sock_, (struct sockaddr*) &client_sockaddr, sizeof(client_sockaddr)))
        {
            close(client_sock_);
            return Error::type::client_bind_failed;
        }

        memset(&server_sockaddr, 0, sizeof(server_sockaddr));
        server_sockaddr.sun_family = AF_UNIX;
        strcpy(server_sockaddr.sun_path, SERVER_PATH);

        if (-1 == connect(client_sock_, (struct sockaddr*) &server_sockaddr, sizeof(server_sockaddr)))
        {
            close(client_sock_);
            return Error::type::client_connect_failed;
        }

        connected_ = true;

        return Error::type::no_error;
    }

//------------------------------------------------------------------------------

    auto request(const std::string &request_string)
    {
        char buf[256];
        auto buf_len = 0;

        buf[0] = '\0';

        if (!connected_) return std::make_tuple(Error::type::client_connect_failed, static_cast<std::string>(buf));

        Log::info(message_string(Message::send) + request_string);

        auto result = send_all(client_sock_, request_string.c_str(), request_string.length());

        auto message = Message::recv;

        if (Error::success(result))
        {
            if (-1 == (buf_len = recv(client_sock_, buf, sizeof(buf), 0)))
            {
                buf_len = 0;
                result = Error::type::client_recv_failed;
                message = Message::recv_fail;
                connected_ = false;
                close(client_sock_);
            }
        } else
        {
            message = Message::send_fail;
            connected_ = false;
            close(client_sock_);
        }

        buf[buf_len] = '\0';

        Log::info(message_string(message) + buf);

        return std::make_tuple(result, static_cast<std::string>(buf));
    }

//------------------------------------------------------------------------------

    auto connected() const
    {
        return connected_;
    }

//------------------------------------------------------------------------------

    static auto message_string(const Client::Message message) -> std::string
    {
        return message_strings_.find(message)->second;
    }

//------------------------------------------------------------------------------

    static auto is_exit(const std::string &message_str)
    {
        return message_string(Message::exit) == message_str;
    }

//------------------------------------------------------------------------------

    static auto send_string(const std::string &send_string)
    {
        return message_string(Message::send, Message::send_color, send_string) +
               message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    static auto recv_string(const std::string &recv_string)
    {
        return message_string(Message::recv, Message::recv_color, recv_string) +
               message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    static auto error_string(const std::string &error_string)
    {
        return message_string(Message::client, Message::error_color, error_string) +
               message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

private:

    static const inline std::map <Client::Message, std::string>
            message_strings_ = {{Message::client,      "Client: "},
                                {Message::command,     "Client: type command << "},
                                {Message::end_command, "\n"},
                                {Message::send,        "Client: send() >> "},
                                {Message::send_fail,   "Client: send() fail "},
                                {Message::recv,        "Client: recv() << "},
                                {Message::recv_fail,   "Client: recv() fail "},
                                {Message::error,       "Client: error << "},
                                {Message::send_color,  "\033[1;32m"},
                                {Message::recv_color,  "\033[1;33m"},
                                {Message::error_color, "\033[1;31m"},
                                {Message::def_color,   "\033[0m"},
                                {Message::exit,        "exit"}};

//------------------------------------------------------------------------------

    Error::type send_all(int sockaddr, const char* buf, int len)
    {
        auto total = 0;
        auto bytesleft = len;
        auto n = 0;

        while (total < len)
        {
            n = send(sockaddr, buf + total, bytesleft, 0);
            if (n == -1) break;
            total += n;
            bytesleft -= n;
        }

        return n == -1 ? Error::type::client_send_failed : Error::type::no_error;
    }

//------------------------------------------------------------------------------

    static auto message_string(Message message, Message color, const std::string &message_string) -> std::string
    {
        return Client::message_string(message) + Client::message_string(color) + message_string +
               Client::message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    const std::string app_name_;
    const std::string sun_path_{"/tmp/"s + app_name_ + ".socket"s};

    int client_sock_ = 0;
    bool connected_ = false;

    Log log_{app_name_};

//------------------------------------------------------------------------------

};  //End of class Client

//------------------------------------------------------------------------------
