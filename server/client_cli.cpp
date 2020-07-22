//------------------------------------------------------------------------------

#include <iostream>

#include "client.h"

//------------------------------------------------------------------------------

int main()
{
    Client client("client_cli");

    auto result = client.init();

    if (!Error::success(result))
    {
        Log::error(Error::strerror(result));
        perror(Error::strerror(result).c_str());
        exit(EXIT_FAILURE);
    }

    std::string request_string;

    while (1)
    {
        std::cout << "Client: type command << ";
        std::getline(std::cin, request_string);

        if(request_string=="exit") return 0;

        request_string += '\n';

        std::cout << "Client: send() >> \033[1;32m" << request_string << "\033[0m";

        auto reply = client.request(request_string);

        if (Error::success(std::get<ERROR_POS>(reply)))
            std::cout << "Client: recv() << \033[1;33m" << std::get<VALUE_POS>(reply) << "\033[0m";
        else
            std::cout << "Client: << \033[31m" << Error::strerror(std::get<ERROR_POS>(reply)) << "\033[0m";
    }

    return 0;
}

//------------------------------------------------------------------------------