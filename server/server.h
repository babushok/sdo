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

    static auto parse_request(std::string request_string)
    {
        std::replace(request_string.begin(), request_string.end(), ',', ' ');
        std::stringstream command_ss{request_string};
        std::vector<decltype(request_string)> result;
        while (command_ss >> request_string) result.push_back(request_string);

        return result;
    }

//------------------------------------------------------------------------------

    auto process_request(std::string request_string)
    {
        Log::info("Server received: "s + request_string);

        auto argv = parse_request(std::move(request_string));

        auto error = Error::no_error;
        auto result = ""s;
        auto command_result = std::make_tuple(error, result);

        if (Command::check_arg_count(argv.size()))
        {
            if (Command::is_command_ex(argv.size()))
                _history.push_back(new Channel_command_ex(&_multimeter, argv[COMMAND_POS], argv[COMMAND_POS + 1],
                                                          argv[COMMAND_POS + 2]));
            else
                _history.push_back(new Channel_command(&_multimeter, argv[COMMAND_POS], argv[COMMAND_POS + 1]));

            command_result = _history.back()->execute();
        } else
            command_result = std::make_tuple(Error::invalid_argument_count, ""s);

        Log::result(command_result);

        return command_result;
    }

//------------------------------------------------------------------------------

    static auto sigchld_handler([[maybe_unused]] int s)
    {
        auto saved_errno = errno;

        while (waitpid(-1, NULL, WNOHANG) > 0);

        errno = saved_errno;
    }

//------------------------------------------------------------------------------

    auto init()
    {
        struct sockaddr_un server_sockaddr;
        struct sigaction sa;
        const int backlog = 10;

        if (-1 == (server_sock = socket(AF_UNIX, SOCK_STREAM, 0))) return Error::server_socket_failed;

        memset(&server_sockaddr, 0, sizeof(server_sockaddr));
        server_sockaddr.sun_family = AF_UNIX;
        strcpy(server_sockaddr.sun_path, _sun_path.c_str());

        unlink(_sun_path.c_str());
        if (-1 == bind(server_sock, (struct sockaddr*) &server_sockaddr, sizeof(server_sockaddr)))
        {
            close(server_sock);
            return Error::server_bind_failed;
        }

        if (-1 == listen(server_sock, backlog))
        {
            close(server_sock);
            return Error::server_listen_failed;
        }

        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) return Error::server_sigaction_failed;

        return Error::no_error;
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
            std::cout << "Server: waiting for connections..." << std::endl;

            addrlen = sizeof client_sockaddr;
            if (-1 == (client_sock = accept(server_sock, (struct sockaddr*) &client_sockaddr, &addrlen)))
            {
                Log::error(Error::strerror(Error::server_accept_failed));
                perror(Error::strerror(Error::server_accept_failed).c_str());
                continue;
            }

            Log::info("Server: accept connection");
            std::cout << "Server: accept connection" << std::endl;

            addrlen = sizeof client_sockaddr;
            if (-1 != getpeername(client_sock, (struct sockaddr*) &client_sockaddr, &addrlen))
            {
                Log::info("Server: client socket filepath: "s + static_cast<std::string>(client_sockaddr.sun_path));
                std::cout << "Server: client socket filepath: " << client_sockaddr.sun_path << std::endl;
            }

            if (!fork())
            {
                close(server_sock);

                while (1)
                {
                    if (-1 == (buf_len = recv(client_sock, buf, sizeof(buf), 0)))
                    {
                        Log::error(Error::strerror(Error::client_recv_failed));
                        perror(Error::strerror(Error::client_recv_failed).c_str());
                    }

                    buf[buf_len] = '\0';
                    std::cout << "Server: recv() << \033[1;32m" << buf << "\033[0m";

                    auto result = process_request(buf);

                    auto reply_string = (Error::success(std::get<0>(result)) ? "ok"s +
                                                                               (std::get<1>(result).length() ?
                                                                                ", "s + std::get<1>(result) : ""s)
                                                                             : "fail"s) + "\n"s;

                    std::cout << "Server: send() >> \033[1;33m" << reply_string <<"\033[0m";

                    if (-1 == send(client_sock, reply_string.c_str(), reply_string.length(), 0))
                    {
                        Log::error(Error::strerror(Error::client_recv_failed));
                        perror(Error::strerror(Error::client_recv_failed).c_str());
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
    const std::string _app_name{SERVER_APP_NAME};
    const std::string _sun_path{SERVER_PATH};

    int server_sock;

    Multimeter _multimeter;
    std::vector<Command*> _history;
    Log _log{_app_name};

//------------------------------------------------------------------------------

};  //End of class Server

//------------------------------------------------------------------------------