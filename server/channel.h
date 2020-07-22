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

    enum class state {
        error_state, idle_state, measure_state, busy_state
    };

//------------------------------------------------------------------------------

    auto set_range(int range)
    {
        if (!check_range(range)) return Error::type::invalid_channel_range;
        _range = range;
        return Error::type::no_error;
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
        if (error_state_val == state) return Channel::state::error_state;
        if (busy_state_val == state) return Channel::state::busy_state;

        return static_cast<Channel::state>(_state);
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
        if (Channel::state::idle_state != get_state()) return Error::type::invalid_channel_state;
        _state = Channel::state::measure_state;
        return Error::type::no_error;
    }

//------------------------------------------------------------------------------

    auto stop_measure()
    {
        if (Channel::state::measure_state != get_state()) return Error::type::invalid_channel_state;
        _state = Channel::state::idle_state;
        return Error::type::no_error;
    }

//------------------------------------------------------------------------------

    static auto channel_state_string(Channel::state state)
    {
        static const std::map<Channel::state, std::string> channel_state_strings = {{state::error_state,   "error_state"},
                                                                                    {state::idle_state,    "idle_state"},
                                                                                    {state::measure_state, "measure_state"},
                                                                                    {state::busy_state,    "busy_state"}};
        return channel_state_strings.find(state)->second;
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
    state _state{state::idle_state};
    int _range{0};

//------------------------------------------------------------------------------

};  //End of class Channel

//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const Channel::state &t)
{
    switch (t)
    {
        case Channel::state::error_state:
            return (out << "error_state");
        case Channel::state::idle_state:
            return (out << "idle_state");
        case Channel::state::measure_state:
            return (out << "measure_state");
        case Channel::state::busy_state:
            return (out << "busy_state");
    }
    return (out);
}

//------------------------------------------------------------------------------