//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include "multimeter.h"

//------------------------------------------------------------------------------

#define MIN_ARGC_COUNT 2
#define COMMAND_POS 0
#define FIRST_ARG_STR "channel"

//------------------------------------------------------------------------------

class Command {

//------------------------------------------------------------------------------

public:

    virtual ~Command() = default;

//------------------------------------------------------------------------------

    virtual auto execute() -> result_t = 0;

//------------------------------------------------------------------------------

    static auto check_arg_count(int argc) { return MIN_ARGC_COUNT <= argc; }

//------------------------------------------------------------------------------

    static auto is_command_ex(int argc) { return MIN_ARGC_COUNT < argc; }

//------------------------------------------------------------------------------

    static auto parse_argument(std::string argument)
    {
        auto result = Error::Type::no_error;
        auto pos = argument.length() - 1;

        while (pos > 0 && std::isdigit(argument[pos]))--pos;

        auto number = 0;

        if (pos++ > 0)
        {
            if (isdigit(argument[pos]))
                number = std::atoi(argument.substr(pos, std::string::npos).c_str());
            else result = Error::Type::invalid_argument;
            argument = argument.substr(0, pos);
        } else result = Error::Type::invalid_argument;
        return std::tuple(result, argument, number);
    }

//------------------------------------------------------------------------------

protected:

    explicit Command(Multimeter &m) : multimeter(m) {}

//------------------------------------------------------------------------------

    Multimeter &multimeter;

//------------------------------------------------------------------------------

};  //End of class Command

//------------------------------------------------------------------------------

class Channel_command : public Command {

//------------------------------------------------------------------------------

public:

    Channel_command(Multimeter &m, std::string command, std::string channel)
            : Command(m), command_(std::move(command)), channel_(std::move(channel)) {}

//------------------------------------------------------------------------------

    auto execute() -> result_t override
    {
	auto [result, channel, channel_number] = parse_argument(channel_);
        if (Error::success(result))
        {
            if (channel != FIRST_ARG_STR) result = Error::Type::invalid_argument;
            else
            {
                auto command = *multimeter.commands()->find(command_);
                if (command_ == command.first) std::tie(result, channel) = command.second(multimeter, channel_number);
                else
                {
                    result = Error::Type::invalid_command;
                    channel = command_;
                }
            }
        }

        return std::tuple(result, channel);
    }

//------------------------------------------------------------------------------

private:

    std::string command_;
    std::string channel_;

//------------------------------------------------------------------------------

};  //End of class Channel_command

//------------------------------------------------------------------------------

class Channel_command_ex : public Command {

//------------------------------------------------------------------------------

public:

    Channel_command_ex(Multimeter &m, std::string command, std::string channel, std::string argument)
            : Command(m), command_(std::move(command)), channel_(std::move(channel)), argument_(std::move(argument)) {}

//------------------------------------------------------------------------------

    auto execute() -> result_t override
    {
        auto argument = ""s;
        auto argument_value = 0;
	auto [result, channel, channel_number] = parse_argument(channel_);
        if (Error::success(result))
        {
            if (channel != FIRST_ARG_STR) result = Error::Type::invalid_argument;
            else
            {
                std::tie(result, argument, argument_value) = parse_argument(argument_);
                if (Error::success(result))
                {
                    if (command_.find(argument) == std::string::npos)
                    {
                        result = Error::Type::invalid_argument;
                        channel = argument;
                    } else
                    {
                        auto command_ex = *multimeter.commands_ex()->find(command_);
                        if (command_ == command_ex.first)
                            std::tie(result, channel) = command_ex.second(multimeter, channel_number, argument_value);
                        else
                        {
                            result = Error::Type::invalid_command;
                            channel = command_;
                        }
                    }
                }
            }
        }

        return std::tuple(result, channel);
    }

//------------------------------------------------------------------------------

private:

    std::string command_;
    std::string channel_;
    std::string argument_;

//------------------------------------------------------------------------------

};  //End of class Channel_command_ex

//------------------------------------------------------------------------------
