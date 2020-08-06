//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <iostream>
#include <random>

#include "error.h"

//------------------------------------------------------------------------------

#define RANGE_COUNT 4

//------------------------------------------------------------------------------

class Channel {

//------------------------------------------------------------------------------

public:

    enum class State {
        error, idle, measure, busy
    };

//------------------------------------------------------------------------------

    auto set_range(int range)
    {
        if (!check_range(range)) return Error::Type::invalid_channel_range;
        range_ = range;
        return Error::Type::no_error;
    }

//------------------------------------------------------------------------------

    [[nodiscard]] auto get_range() const { return range_; }

//------------------------------------------------------------------------------

    [[nodiscard]] auto get_state() const
    {
        const auto error_state_val = 3;
        const auto busy_state_val = 7;

        const auto bound0 = 0;
        const auto bound1 = 9;

        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(bound0, bound1);

        auto state = distribution(generator);
        if (error_state_val == state) return Channel::State::error;
        if (busy_state_val == state) return Channel::State::busy;

        return static_cast<Channel::State>(state_);
    };

//------------------------------------------------------------------------------

    [[nodiscard]] auto get_value() const
    {
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_real_distribution<float> distribution(range_bounds_[range_][0], range_bounds_[range_][1]);

        return distribution(generator);
    }

//------------------------------------------------------------------------------

    [[nodiscard]] auto is_measure_state() const
    {
        return (Channel::State::measure == get_state());
    }

//------------------------------------------------------------------------------

    [[nodiscard]] auto is_idle_state() const
    {
        return (Channel::State::idle == get_state());
    }

//------------------------------------------------------------------------------

    auto start_measure()
    {
        if (!is_idle_state()) return Error::Type::invalid_channel_state;
        state_ = Channel::State::measure;
        return Error::Type::no_error;
    }

//------------------------------------------------------------------------------

    auto stop_measure()
    {
        if (!is_measure_state()) return Error::Type::invalid_channel_state;
        state_ = Channel::State::idle;
        return Error::Type::no_error;
    }

//------------------------------------------------------------------------------

    static auto channel_state_string(Channel::State state)
    {
        return channel_state_strings_.find(state)->second;
    }

//------------------------------------------------------------------------------

private:

    static auto check_range(int range) -> bool
    {
        return (range >= 0 || range < RANGE_COUNT);
    }

//------------------------------------------------------------------------------
    static const inline std::map <Channel::State, std::string> channel_state_strings_ = {{State::error,   "error_state"},
                                                                                        {State::idle,    "idle_state"},
                                                                                        {State::measure, "measure_state"},
                                                                                        {State::busy,    "busy_state"}};
    const float range_bounds_[RANGE_COUNT][2]{{0.000'000'1, 0.000'999},
                                             {0.001,       0.999},
                                             {1,           999},
                                             {1'000,       999'999}};
    State state_{State::idle};
    int range_{0};

//------------------------------------------------------------------------------

};  //End of class Channel

//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const Channel::State &t)
{
    switch (t)
    {
        case Channel::State::error:
            return (out << "error_state");
        case Channel::State::idle:
            return (out << "idle_state");
        case Channel::State::measure:
            return (out << "measure_state");
        case Channel::State::busy:
            return (out << "busy_state");
    }
    return (out);
}

//------------------------------------------------------------------------------
