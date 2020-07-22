//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <array>
#include <map>
#include <functional>

#include "common.h"
#include "channel.h"

//------------------------------------------------------------------------------

class Multimeter {

//------------------------------------------------------------------------------

public:

    using func_t = std::function<result_t(Multimeter &, int)>;
    using func_ex_t = std::function<result_t(Multimeter &, int, int)>;

//------------------------------------------------------------------------------

    Multimeter() { bind_commands(); }

//------------------------------------------------------------------------------

    auto set_channel_range(int channel_number, int range)
    {
        const auto range_string = "range"s;
        auto result = Error::no_error;

        if (!check_channel_number(channel_number)) result = Error::invalid_channel_number;
        else
        {
            result = static_cast<Error::error>(channels[channel_number].set_range(range));
            range = channels[channel_number].get_range();
        }

        return std::make_tuple(result, range_string + std::to_string(range));
    }

//------------------------------------------------------------------------------

    auto start_channel_measure(int channel_number)
    {
        auto result = Error::no_error;
        if (!check_channel_number(channel_number)) result = Error::invalid_channel_number;
        else result = channels[channel_number].start_measure();
        return std::make_tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    auto stop_channel_measure(int channel_number)
    {
        auto result = Error::no_error;
        if (!check_channel_number(channel_number)) result = Error::invalid_channel_number;
        else result = channels[channel_number].stop_measure();
        return std::make_tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    auto get_channel_state(int channel_number)
    {
        auto result = Error::no_error;
        auto state = ""s;

        if (!check_channel_number(channel_number)) result = Error::invalid_channel_number;
        else
            state = Channel::channel_state_string(channels[channel_number].get_state());

        return std::make_tuple(result, state);
    }

//------------------------------------------------------------------------------

    auto get_channel_value(int channel_number)
    {
        auto result = Error::no_error;
        auto value = ""s;

        if (!check_channel_number(channel_number)) result = Error::invalid_channel_number;
        else
        {
            result = (Channel::measure_state == channels[channel_number].get_state())
                     ? Error::no_error
                     : Error::invalid_channel_state;
            value = std::to_string(channels[channel_number].get_value());
            value = value.substr(0, 2) == "0." ? value.substr(0, 9) : value.substr(0, value.find('.'));
        }

        return std::make_tuple(result, value);
    }

//------------------------------------------------------------------------------

    [[nodiscard]] const std::map<std::string, func_t>* commands() const { return &_commands; }

    [[nodiscard]] const std::map<std::string, func_ex_t>* commands_ex() const { return &_commands_ex; }

//------------------------------------------------------------------------------

private:

    static auto check_channel_number(int channel_number) -> bool
    {
        return (channel_number >= 0 && channel_number < CHANNEL_COUNT);
    }

//------------------------------------------------------------------------------

    void bind_commands()
    {
        _commands.insert(std::make_pair("start_measure"s, &Multimeter::start_channel_measure));
        _commands.insert(std::make_pair("stop_measure"s, &Multimeter::stop_channel_measure));

        _commands_ex.insert(std::make_pair("set_range", &Multimeter::set_channel_range));

        _commands.insert(std::make_pair("get_status", &Multimeter::get_channel_state));
        _commands.insert(std::make_pair("get_result", &Multimeter::get_channel_value));
    }

//------------------------------------------------------------------------------

    std::map<std::string, func_t> _commands;
    std::map<std::string, func_ex_t> _commands_ex;

    std::array<Channel, CHANNEL_COUNT> channels;

//------------------------------------------------------------------------------

};  //End of class Multimeter

//------------------------------------------------------------------------------