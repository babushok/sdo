//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#include "log.h"
#include "common.h"

//------------------------------------------------------------------------------

class Client {

//------------------------------------------------------------------------------

public:

    Client(std::string app_name) : _app_name{app_name}, _sun_path{"/tmp/"s + app_name + ".socket"s}, _log{app_name} {}

//------------------------------------------------------------------------------

    auto init()
    {
        struct sockaddr_un client_sockaddr;
        struct sockaddr_un server_sockaddr;

        if (-1 == (_client_sock = socket(AF_UNIX, SOCK_STREAM, 0))) return Error::type::client_socket_failed;

        memset(&client_sockaddr, 0, sizeof(client_sockaddr));
        client_sockaddr.sun_family = AF_UNIX;
        strcpy(client_sockaddr.sun_path, _sun_path.c_str());

        unlink(_sun_path.c_str());
        if (-1 == bind(_client_sock, (struct sockaddr*) &client_sockaddr, sizeof(client_sockaddr)))
        {
            close(_client_sock);
            return Error::type::client_bind_failed;
        }

        memset(&server_sockaddr, 0, sizeof(server_sockaddr));
        server_sockaddr.sun_family = AF_UNIX;
        strcpy(server_sockaddr.sun_path, SERVER_PATH);

        if (-1 == connect(_client_sock, (struct sockaddr*) &server_sockaddr, sizeof(server_sockaddr)))
        {
            close(_client_sock);
            return Error::type::client_connect_failed;
        }

        return Error::type::no_error;
    }

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

    auto request(std::string request_string)
    {
        char buf[256];
        auto buf_len = 0;

        buf[0] = '\0';

        auto result = send_all(_client_sock, request_string.c_str(), request_string.length());

        if (Error::success(result))
            if (-1 == (buf_len = recv(_client_sock, buf, sizeof(buf), 0)))
            {
                close(_client_sock);
                result = Error::type::client_recv_failed;
            }

        buf[buf_len] = '\0';

        return std::make_tuple(result, static_cast<std::string>(buf));
    }

//------------------------------------------------------------------------------

private:
    std::string _app_name;
    std::string _sun_path;

    int _client_sock;

    Log _log;

//------------------------------------------------------------------------------

};  //End of class Client

//------------------------------------------------------------------------------