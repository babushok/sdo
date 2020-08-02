//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#include "command.h"
#include "log.h"

//------------------------------------------------------------------------------

class Server {

//------------------------------------------------------------------------------

public:

    auto init()
    {
        struct sockaddr_un server_sockaddr;
        struct sigaction sa;
        const int backlog = 10;

        if (-1 == (server_sock = socket(AF_UNIX, SOCK_STREAM, 0))) return Error::type::server_socket_failed;

        memset(&server_sockaddr, 0, sizeof(server_sockaddr));
        server_sockaddr.sun_family = AF_UNIX;
        strcpy(server_sockaddr.sun_path, _sun_path.c_str());

        unlink(_sun_path.c_str());
        if (-1 == bind(server_sock, (struct sockaddr*) &server_sockaddr, sizeof(server_sockaddr)))
        {
            close(server_sock);
            return Error::type::server_bind_failed;
        }

        if (-1 == listen(server_sock, backlog))
        {
            close(server_sock);
            return Error::type::server_listen_failed;
        }

        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) return Error::type::server_sigaction_failed;

        return Error::type::no_error;
    }

//------------------------------------------------------------------------------

    auto run()
    {
        int client_sock;
        struct sockaddr_un client_sockaddr;
        socklen_t addrlen;

        char buf[256];
        auto buf_len = 0;

        while (1)
        {
            std::cout << message_string(Message::waiting) << std::endl;

            addrlen = sizeof client_sockaddr;
            if (-1 == (client_sock = accept(server_sock, (struct sockaddr*) &client_sockaddr, &addrlen)))
            {
                Log::error(Error::strerror(Error::type::server_accept_failed));
                perror(Error::strerror(Error::type::server_accept_failed).c_str());
                continue;
            }

            Log::info(message_string(Message::accept));
            std::cout << message_string(Message::accept) << std::endl;

            addrlen = sizeof client_sockaddr;
            if (-1 != getpeername(client_sock, (struct sockaddr*) &client_sockaddr, &addrlen))
            {
                Log::info(message_string(Message::client_path) + static_cast<std::string>(client_sockaddr.sun_path));
                std::cout << message_string(Message::client_path) << client_sockaddr.sun_path << std::endl;
            }

            if (!fork())
            {
                close(server_sock);

                while (1)
                {
                    if (-1 == (buf_len = recv(client_sock, buf, sizeof(buf), 0)))
                    {
                        Log::error(Error::strerror(Error::type::client_recv_failed));
                        perror(Error::strerror(Error::type::client_recv_failed).c_str());
                    }

                    buf[buf_len] = '\0';

                    std::cout << recv_string(buf);

                    auto result = process_request(buf);

                    auto reply_string = (Error::success(result) ? message_string(Message::ok) +
                                                                  (Error::value(result).length() ?
                                                                   ", "s + Error::value(result) : ""s)
                                                                : message_string(Message::fail)) +
                                        message_string(Message::end_command);

                    std::cout << send_string(reply_string);

                    if (!Error::success(send_all(client_sock, reply_string.c_str(), reply_string.length())))
                    {
                        Log::error(Error::strerror(Error::type::client_send_failed));
                        perror(Error::strerror(Error::type::client_send_failed).c_str());
                    }
                }

                close(client_sock);

                exit(0);
            }
            close(client_sock);  // parent doesn't need this
        }
    }

//------------------------------------------------------------------------------

private:

    enum class Message {
        received, waiting, accept, client_path, recv, send, ok, fail, end_command, recv_color, send_color, def_color
    };

    static const inline std::map <Server::Message, std::string>
            message_strings = {{Message::received,    "Server received: "},
                               {Message::waiting,     "Server: waiting for connections..."},
                               {Message::accept,      "Server: accept connection"},
                               {Message::client_path, "Server: client socket filepath: "},
                               {Message::recv,        "Server: recv() << "},
                               {Message::send,        "Server: send() >> "},
                               {Message::ok,          "ok"},
                               {Message::fail,        "fail"},
                               {Message::end_command, "\n"},
                               {Message::recv_color,  "\033[1;32m"},
                               {Message::send_color,  "\033[1;33m"},
                               {Message::def_color,   "\033[0m"}};


    const std::string _app_name{SERVER_APP_NAME};
    const std::string _sun_path{SERVER_PATH};

//------------------------------------------------------------------------------

    static auto parse_request(std::string request_string)
    {
        std::replace(request_string.begin(), request_string.end(), ',', ' ');
        std::stringstream command_ss{request_string};
        std::vector<decltype(request_string)> result;
        while (command_ss >> request_string) result.push_back(request_string);

        return result;
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

    static auto sigchld_handler([[maybe_unused]] int s) -> void
    {
        auto saved_errno = errno;

        while (waitpid(-1, NULL, WNOHANG) > 0);

        errno = saved_errno;
    }

//------------------------------------------------------------------------------

    auto process_request(const std::string &request_string) -> result_t
    {
        Log::info(message_string(Message::received) + request_string);

        auto argv = parse_request(std::move(request_string));

        auto error = Error::type::no_error;
        auto result = ""s;
        auto command_result = std::make_tuple(error, result);

        if (Command::check_arg_count(argv.size()))
        {
            if (Command::is_command_ex(argv.size()))
                _history.push_back(new Channel_command_ex(_multimeter, argv[COMMAND_POS], argv[COMMAND_POS + 1],
                                                          argv[COMMAND_POS + 2]));
            else
                _history.push_back(new Channel_command(_multimeter, argv[COMMAND_POS], argv[COMMAND_POS + 1]));

            command_result = _history.back()->execute();
        } else
            command_result = std::make_tuple(Error::type::invalid_argument_count, ""s);

        Log::result(command_result);

        return command_result;
    }

//------------------------------------------------------------------------------

    static auto message_string(Server::Message message) -> std::string
    {
        return message_strings.find(message)->second;
    }

//------------------------------------------------------------------------------

    static auto message_string(Message message, Message color, const std::string &message_string) -> std::string
    {
        return Server::message_string(message) + Server::message_string(color) + message_string +
               Server::message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    static auto send_string(const std::string &send_string) -> std::string
    {
        return message_string(Message::send, Message::send_color, send_string) +
               message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    static auto recv_string(const std::string &recv_string) -> std::string
    {
        return message_string(Message::recv, Message::recv_color, recv_string) +
               message_string(Message::def_color);
    }

//------------------------------------------------------------------------------

    int server_sock;

    Multimeter _multimeter;
    std::vector<Command*> _history;
    Log _log{_app_name};

//------------------------------------------------------------------------------

};  //End of class Server

//------------------------------------------------------------------------------