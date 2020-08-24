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

    result_t set_channel_range(int channel_number, int range)
    {
        const auto range_string = "range"s;
        auto result = Error::Type::no_error;

        if (!check_channel_number(channel_number)) result = Error::Type::invalid_channel_number;
        else
        {
            result = static_cast<Error::Type>(channels_[channel_number].set_range(range));
            range = channels_[channel_number].get_range();
        }

        return std::tuple(result, range_string + std::to_string(range));
    }

//------------------------------------------------------------------------------

    result_t start_channel_measure(int channel_number)
    {
        auto result = Error::Type::no_error;
        if (!check_channel_number(channel_number)) result = Error::Type::invalid_channel_number;
        else result = channels_[channel_number].start_measure();
        return std::tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    result_t stop_channel_measure(int channel_number)
    {
        auto result = Error::Type::no_error;
        if (!check_channel_number(channel_number)) result = Error::Type::invalid_channel_number;
        else result = channels_[channel_number].stop_measure();
        return std::tuple(result, ""s);
    }

//------------------------------------------------------------------------------

    result_t get_channel_state(int channel_number)
    {
        auto result = Error::Type::no_error;
        auto state = ""s;

        if (!check_channel_number(channel_number)) result = Error::Type::invalid_channel_number;
        else
            state = Channel::channel_state_string(channels_[channel_number].get_state());

        return std::tuple(result, state);
    }

//------------------------------------------------------------------------------

    result_t get_channel_value(int channel_number)
    {
        auto result = Error::Type::no_error;
        auto value = ""s;

        if (!check_channel_number(channel_number)) result = Error::Type::invalid_channel_number;
        else
        {
            result = channels_[channel_number].is_measure_state()
                     ? Error::Type::no_error
                     : Error::Type::invalid_channel_state;
            value = std::to_string(channels_[channel_number].get_value());
            value = value.substr(0, 2) == "0." ? value.substr(0, 9) : value.substr(0, value.find('.'));
        }

        return std::tuple(result, value);
    }

//------------------------------------------------------------------------------

    [[nodiscard]] const std::map <std::string, func_t>* commands() const { return &commands_; }

    [[nodiscard]] const std::map <std::string, func_ex_t>* commands_ex() const { return &commands_ex_; }

//------------------------------------------------------------------------------

private:

    static auto check_channel_number(int channel_number) -> bool
    {
        return (channel_number >= 0 && channel_number < CHANNEL_COUNT);
    }

//------------------------------------------------------------------------------

    const std::map <std::string, func_t> commands_ = {{"start_measure", &Multimeter::start_channel_measure},
                                                      {"stop_measure",  &Multimeter::stop_channel_measure},
                                                      {"get_status",    &Multimeter::get_channel_state},
                                                      {"get_result",    &Multimeter::get_channel_value}};
    const std::map <std::string, func_ex_t> commands_ex_ = {{"set_range", &Multimeter::set_channel_range}};

    std::array<Channel, CHANNEL_COUNT> channels_;

//------------------------------------------------------------------------------

};  //End of class Multimeter

//------------------------------------------------------------------------------
