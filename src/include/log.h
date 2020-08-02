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
        return (Error::success(log_result) ? message_string(Message::ok) : message_string(Message::fail)) +
               (Error::value(log_result).length() ? ", "s + Error::value(log_result) : ""s);
    }

//------------------------------------------------------------------------------

    static auto result(result_t log_result)
    {
        if (Error::success(log_result))
            info(message_string(Message::processing) + Error::strerror(log_result));
        else
            Log::error(message_string(Message::processing_fail) + Error::strerror(log_result));

        info(message_string(Message::result) + make_reply_string(log_result));
    }

//------------------------------------------------------------------------------

private:

    enum class Message {
        processing, processing_fail, result, ok, fail
    };

//------------------------------------------------------------------------------

    static const inline std::map <Log::Message, std::string>
            message_strings_ = {{Message::processing,      "Request processing: "},
                               {Message::processing_fail, "Request processing fail: "},
                               {Message::result,          "Command result: "},
                               {Message::ok,              "ok"},
                               {Message::fail,            "fail"}};

//------------------------------------------------------------------------------

    static auto message_string(Log::Message message) -> std::string
    {
        return message_strings_.find(message)->second;
    }

//------------------------------------------------------------------------------

};  //End of class Log

//------------------------------------------------------------------------------