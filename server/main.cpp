//------------------------------------------------------------------------------

#include "server.h"

//------------------------------------------------------------------------------

int main()
{
    Server server;

    auto result = server.init();

    if (!Error::success(result))
    {
        Log::error(Error::strerror(result));
        perror(Error::strerror(result).c_str());
        if (result != Error::type::server_sigaction_failed) exit(EXIT_FAILURE);
    }

    server.run();

    return 0;
}

//------------------------------------------------------------------------------