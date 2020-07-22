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

    auto set_channel_range(int channel_number, int range)
    {
        const auto range_string = "range"s;
        auto result = Error::type::no_error;

        if (!check_channel_number(channel_number)) result = Error::type::invalid_channel_number;
        else
        {
            result = static_cast<Error::type>(channels[channel_number].set_range(range));
            range = channels[channel_number].get_range();
        }

        return std::make_tuple(result, range_string + std::to_string(range));
    }

//------------------------------------------------------------------------------

    auto start_channel_measure(int channel_number)
    {
        auto result = Error::type::no_error;
        if (!check_channel_number(channel_number)) result = Error::type::invalid_channel_number;
        else result = channels[channel_number].start_measure();
        return std::make_tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    auto stop_channel_measure(int channel_number)
    {
        auto result = Error::type::no_error;
        if (!check_channel_number(channel_number)) result = Error::type::invalid_channel_number;
        else result = channels[channel_number].stop_measure();
        return std::make_tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    auto get_channel_state(int channel_number)
    {
        auto result = Error::type::no_error;
        auto state = ""s;

        if (!check_channel_number(channel_number)) result = Error::type::invalid_channel_number;
        else
            state = Channel::channel_state_string(channels[channel_number].get_state());

        return std::make_tuple(result, state);
    }

//------------------------------------------------------------------------------

    auto get_channel_value(int channel_number)
    {
        auto result = Error::type::no_error;
        auto value = ""s;

        if (!check_channel_number(channel_number)) result = Error::type::invalid_channel_number;
        else
        {
            result = (Channel::state::measure_state == channels[channel_number].get_state())
                     ? Error::type::no_error
                     : Error::type::invalid_channel_state;
            value = std::to_string(channels[channel_number].get_value());
            value = value.substr(0, 2) == "0." ? value.substr(0, 9) : value.substr(0, value.find('.'));
        }

        return std::make_tuple(result, value);
    }

//------------------------------------------------------------------------------

    Multimeter()
    {
        _commands = {{"start_measure", &Multimeter::start_channel_measure},
                     {"stop_measure",  &Multimeter::stop_channel_measure},
                     {"get_status",    &Multimeter::get_channel_state},
                     {"get_result",    &Multimeter::get_channel_value}};

        _commands_ex = {{"set_range", &Multimeter::set_channel_range}};
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

    std::map<std::string, func_t> _commands;
    std::map<std::string, func_ex_t> _commands_ex;

    std::array<Channel, CHANNEL_COUNT> channels;

//------------------------------------------------------------------------------

};  //End of class Multimeter

//------------------------------------------------------------------------------