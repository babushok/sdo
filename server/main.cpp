#include "server.h"

int main()
{
//--Win32------------------------------
//    init_syslog("localhost");
//-------------------------------------

    Server server;

    auto result = server.init();

    if (!Error::success(result))
    {
        Log::error(Error::strerror(result));
        perror(Error::strerror(result).c_str());
        if (result != Error::server_sigaction_failed) exit(EXIT_FAILURE);
    }

    server.run();

//--Win32------------------------------
//    exit_syslog();
//-------------------------------------

    return 0;
}
