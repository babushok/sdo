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

    enum channel_state {
        error_state, idle_state, measure_state, busy_state
    };

//------------------------------------------------------------------------------

    auto set_range(int range)
    {
        if (!check_range(range)) return Error::invalid_channel_range;
        _range = range;
        return Error::no_error;
    }

//------------------------------------------------------------------------------

    [[nodiscard]] auto get_range() const { return _range; }

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
        if (error_state_val == state) return Channel::error_state;
        if (busy_state_val == state) return Channel::busy_state;

        return static_cast<channel_state>(_state);
    };

//------------------------------------------------------------------------------

    [[nodiscard]] auto get_value() const
    {
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_real_distribution<float> distribution(range_bounds[_range][0], range_bounds[_range][1]);

        return distribution(generator);
    }

//------------------------------------------------------------------------------

    auto start_measure()
    {
        if (Channel::idle_state != get_state()) return Error::invalid_channel_state;
        _state = Channel::measure_state;
        return Error::no_error;
    }

//------------------------------------------------------------------------------

    auto stop_measure()
    {
        if (Channel::measure_state != get_state()) return Error::invalid_channel_state;
        _state = Channel::idle_state;
        return Error::no_error;
    }

//------------------------------------------------------------------------------

    static auto channel_state_string(channel_state state)
    {
        static const char* channel_state_strings[] = {"error_state", "idle_state", "measure_state", "busy_state"};
        return channel_state_strings[state];
    }

//------------------------------------------------------------------------------

private:

    static auto check_range(int range) -> bool
    {
        return (range >= 0 || range < RANGE_COUNT);
    }

//------------------------------------------------------------------------------

    const float range_bounds[RANGE_COUNT][2]{{0.000'000'1, 0.000'999},
                                             {0.001,       0.999},
                                             {1,           999},
                                             {1'000,       999'999}};
    channel_state _state{idle_state};
    int _range{0};

//------------------------------------------------------------------------------

};  //End of class Channel

//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const Channel::channel_state &t)
{
    switch (t)
    {
        case Channel::error_state:
            return (out << "error_state");
        case Channel::idle_state:
            return (out << "idle_state");
        case Channel::measure_state:
            return (out << "measure_state");
        case Channel::busy_state:
            return (out << "busy_state");
    }
    return (out);
}

//------------------------------------------------------------------------------