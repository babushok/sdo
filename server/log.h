//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <string>

#include "syslog.h"
#include "error.h"

//------------------------------------------------------------------------------

struct Log {

    explicit Log(const std::string &app_name)
    {
        openlog(app_name.c_str(), LOG_PID, LOG_USER);
    }

//------------------------------------------------------------------------------

    ~Log() { closelog(); }

//------------------------------------------------------------------------------

    static auto error(const std::string &log_string)
    {
        syslog(LOG_ERR, "%s", log_string.c_str());
    }

//------------------------------------------------------------------------------

    static auto warning(const std::string &log_string)
    {
        syslog(LOG_WARNING, "%s", log_string.c_str());
    }

//------------------------------------------------------------------------------

    static auto notice(const std::string &log_string)
    {
        syslog(LOG_NOTICE, "%s", log_string.c_str());
    }

//------------------------------------------------------------------------------

    static auto info(const std::string &log_string)
    {
        syslog(LOG_INFO, "%s", log_string.c_str());
    }

//------------------------------------------------------------------------------

    static auto make_reply_string(result_t log_result)
    {
        return (std::get<0>(log_result) == Error::no_error ? "ok"s : "fail"s) +
               (std::get<1>(log_result).length() ? ", "s + std::get<1>(log_result) : ""s);
    }

//------------------------------------------------------------------------------

    static auto result(result_t log_result)
    {
        auto error = std::get<0>(log_result);

        if (Error::success(error))
            info("Request processing: "s + Error::strerror(error));
        else
            Log::error("Request processing fail: "s + Error::strerror(error));

        info("Command result: "s + make_reply_string(log_result));
    }

//------------------------------------------------------------------------------

};  //End of class Log

//------------------------------------------------------------------------------