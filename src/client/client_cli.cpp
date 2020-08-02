//------------------------------------------------------------------------------

#include <iostream>

#include "include/client.h"

//------------------------------------------------------------------------------

#define CLIENT_CLI_APP_NAME "client_cli"

//------------------------------------------------------------------------------

int main()
{
    Client client(CLIENT_CLI_APP_NAME);

    auto result = client.init();

    if (!Error::success(result))
    {
        Log::error(Error::strerror(result));
        perror(Error::strerror(result).c_str());
        exit(EXIT_FAILURE);
    }

    std::string request_string;

    while (true)
    {
        std::cout << Client::message_string(Client::Message::command);
        std::getline(std::cin, request_string);

        if(Client::is_exit(request_string)) return 0;

        request_string += Client::message_string(Client::Message::end_command);

        std::cout << Client::send_string(request_string);

        auto reply = client.request(request_string);

        if (Error::success(reply))
            std::cout << Client::recv_string(Error::value(reply));
        else
            std::cout << Client::error_string(Error::strerror(reply));
    }

    return 0;
}

//------------------------------------------------------------------------------