//------------------------------------------------------------------------------

#include <QSettings>
#include <sstream>

#include "include/multimeter.h"

//------------------------------------------------------------------------------

bool Multimeter::is_error_state(QString state)
{
    return state==Channel::state_strings[Channel::State::error_state];
}

//------------------------------------------------------------------------------

bool Multimeter::is_idle_state(QString state)
{
    return state==Channel::state_strings[Channel::State::idle_state];
}

//------------------------------------------------------------------------------

bool Multimeter::is_measure_state(QString state)
{
    return state==Channel::state_strings[Channel::State::measure_state];
}

//------------------------------------------------------------------------------

bool Multimeter::is_busy_state(QString state)
{
    return state==Channel::state_strings[Channel::State::busy_state];
}

//------------------------------------------------------------------------------

bool Multimeter::is_available_channels()
{
    return available_channel_list().size();
}

//------------------------------------------------------------------------------

bool Multimeter::is_busy_channels()
{
    return busy_channel_list().size();
}

//------------------------------------------------------------------------------

Multimeter::~Multimeter()
{
    if(client_.connected()) set_settings();
}

//------------------------------------------------------------------------------

void Multimeter::set_channel_count()
{
    auto channel_count = 0;
    Channel channel;
    channel.number = channel_count;

    while(channel.get_state(client_) && channel_count<max_channel_count)
        channel.number = ++channel_count;

    channel_count_ = channel_count;
}

//------------------------------------------------------------------------------

Error::type Multimeter::init()
{
    auto result = client_.init();

    if(Error::success(result))
    {
        set_channel_count();

        get_settings();

        set_channel_list_model();
    }

    return result;
}

//------------------------------------------------------------------------------


QVector<Channel> Multimeter::channels() const
{
    return channels_;
}

//------------------------------------------------------------------------------

bool Multimeter::set_channel_at(int index, const Channel &channel)
{
    if (index < 0 || index >= channels_.size())
        return false;

    const Channel &oldItem = channels_.at(index);
    if (channel.number == oldItem.number
            && channel.measure_state == oldItem.measure_state
            && channel.voltage == oldItem.voltage
            && channel.range == oldItem.range
            && channel.state == oldItem.state)
        return false;

    auto state_changed = channel.measure_state!=oldItem.measure_state;
    auto range_changed = channel.range!=oldItem.range;

    auto range = channels_[index].range;
    auto measure_state = channels_[index].measure_state;

    channels_[index] = channel;

    if(state_changed) if(!channels_[index].set_measure_state(client_)) channels_[index].measure_state = measure_state;
    if(range_changed) if(!channels_[index].set_range(client_)) channels_[index].range = range;

    return true;
}

//------------------------------------------------------------------------------

void Multimeter::append_channel(int channel_number)
{
    Channel channel;
    channel.number = channel_number;
    channel.measure_state = false;
    channel.voltage = Channel::voltage_str;
    channel.range = Channel::default_range;
    channel.state = Channel::state_strings[Channel::State::idle_state];

    auto index = 0;
    while(index<channels_.size()&&channels_[index].number<channel_number)++index;

    emit pre_channel_append(index);

    channel.set_measure_state(client_);
    channel.set_range(client_);

    channels_.insert(index, channel);

    emit post_channel_append();

    set_channel_list_model();
}

//------------------------------------------------------------------------------

void Multimeter::remove_channel(int channel_number)
{
    auto channel = std::find_if(channels_.begin(), channels_.end(),
                                [=](Channel const& channel) -> bool {return channel.number == channel_number;});

    if(channel != channels_.end())
    {
        emit pre_channel_removed(std::distance(channels_.begin(), channel));

        channels_.erase(channel);

        emit post_channel_removed();

        set_channel_list_model();
    }
}

//------------------------------------------------------------------------------

QStringList Multimeter::available_channel_list()
{
    QStringList channels_list;

    for(auto channel_number=0; channel_number<get_channel_count(); ++channel_number)
    {
        auto present = false;
        for(auto& channel:channels_) if((present=(channel.number==channel_number))) break;
        if(!present) channels_list << QString::number(channel_number);
    }

    return channels_list;
}

//------------------------------------------------------------------------------

QStringList Multimeter::busy_channel_list()
{
    QStringList channels_list;

    for(auto channel_number=0; channel_number<get_channel_count(); ++channel_number)
    {
        auto busy = false;
        for(auto& channel:channels_) if((busy=(channel.number==channel_number))) break;
        if(busy) channels_list << QString::number(channel_number);
    }

    return channels_list;
}

//------------------------------------------------------------------------------

void Multimeter::set_channel_list_model()
{
    available_channel_list_model_.setStringList(available_channel_list());
    busy_channel_list_model_.setStringList(busy_channel_list());
}

//------------------------------------------------------------------------------

void Multimeter::get_settings()
{
    QSettings settings(settings_strings[Settings::company], settings_strings[Settings::product]);
    auto size = settings.beginReadArray(settings_strings[Settings::channels]);
    for (auto i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Channel channel;
        channel.number = settings.value(settings_strings[Settings::number]).toInt();
        channel.voltage = Channel::voltage_str;
        channel.range = static_cast<Channel::Range>(settings.value(settings_strings[Settings::range]).toInt());

        channel.measure_state = false;

        const auto max_attempts = 5;
        auto attempt = 0;

        channel.get_state(client_);

        while(!(channel.state==Channel::state_strings[Channel::State::idle_state]||
                channel.state==Channel::state_strings[Channel::State::measure_state])
              && max_attempts>++attempt) channel.get_state(client_);

        channel.measure_state = channel.state==Channel::state_strings[Channel::State::measure_state];

        channel.set_range(client_);

        channels_.append(channel);
    }

    settings.endArray();
}

//------------------------------------------------------------------------------

void Multimeter::set_settings()
{
    QSettings settings(settings_strings[Settings::company], settings_strings[Settings::product]);
    settings.beginWriteArray(settings_strings[Settings::channels]);
    auto index = 0;
    for (const auto& channel: channels_)
    {
        settings.setArrayIndex(index++);
        settings.setValue(settings_strings[Settings::number], channel.number);
        settings.setValue(settings_strings[Settings::measure_state], channel.measure_state);
        settings.setValue(settings_strings[Settings::voltage], channel.voltage);
        settings.setValue(settings_strings[Settings::range], static_cast<int>(channel.range));
    }
    settings.endArray();
    settings.sync();
}

//------------------------------------------------------------------------------

bool Multimeter::get_data()
{
    for(auto& channel : channels_)
    {
        channel.get_state(client_);
        if(!client_.connected()) return false;
        if(channel.measure_state) channel.get_voltage(client_);
    }

    emit data_changed();

    return true;
}

//------------------------------------------------------------------------------

bool Channel::request_success(const result_t &reply)
{
    return Error::value(reply).substr(0, command_strings[Command::ok].size()) == command_strings[Command::ok].toStdString();
}

//------------------------------------------------------------------------------

std::vector<std::string> Channel::parse_reply(const result_t &reply)
{
    auto reply_string = Error::value(reply);
    std::replace(reply_string.begin(), reply_string.end(), ',', ' ');
    std::stringstream command_ss{reply_string};
    std::vector<decltype(reply_string)> result;
    while (command_ss >> reply_string) result.push_back(reply_string);

    return result;
}

//------------------------------------------------------------------------------

QString Channel::command_string(Channel::Command command)
{
    auto command_string = command_strings[command];

    command_string.append(" ").append(command_strings[Command::channel]).append(QString::number(number)).append('\n');

    return command_string;
}

//------------------------------------------------------------------------------

QString Channel::command_string(Channel::Command command, Channel::Command arg, int argv)
{
    auto command_string = command_strings[command];

    command_string.append(" ").append(command_strings[Command::channel]).append(QString::number(number));
    command_string.append(", ").append(command_strings[arg]).append(QString::number(argv)).append('\n');

    return command_string;
}

//------------------------------------------------------------------------------

bool Channel::set_measure_state(Client &client)
{
    bool result = false;

    auto request = command_string(measure_state ? Command::start_measure : Command::stop_measure);

    auto reply = client.request(request.toStdString());

    if(Error::success(reply))
    {
        const auto max_attempts = 3;
        auto attempt = 0;

        while(!request_success(reply) && max_attempts>++attempt)
            reply = client.request(request.toStdString());

        result = request_success(reply);
    }

    get_state(client);

    return result;
}

//------------------------------------------------------------------------------

bool Channel::get_voltage(Client &client)
{
    auto result = false;

    auto reply = client.request(command_string(Command::get_result).toStdString());

    auto argv = parse_reply(reply);

    const auto argc = 2;
    const auto argn = 1;

    if(Error::success(reply)
            && request_success(reply)
            && argc <= argv.size())
    {
        voltage = QString::fromStdString(argv[argn]);
        result = true;
    }

    return result;
}

//------------------------------------------------------------------------------

bool Channel::get_state(Client &client)
{
    auto result = false;

    auto reply = client.request(command_string(Command::get_status).toStdString());

    auto argv = parse_reply(reply);

    const auto argc = 2;
    const auto argn = 1;

    if(Error::success(reply)
            && request_success(reply)
            && argc <= argv.size())
    {
        QString get_state_string = QString::fromStdString(argv[argn]);
        for(auto& state_string: state_strings)
            if(get_state_string.contains(state_string)) {
                state = state_string;
                result = true;
                break;
            }
    }


    return result;
}

//------------------------------------------------------------------------------

bool Channel::set_range(Client &client)
{
    bool result = false;

    auto reply = client.request(command_string(Command::set_range, Command::range, static_cast<int>(range)).toStdString());

    auto argv = parse_reply(reply);

    const auto argc = 2;
    const auto argn = 1;

    if(Error::success(reply)
            && request_success(reply)
            && argc <= argv.size())
    {
        QString get_range_string = QString::fromStdString(argv[argn]).right(argn);
        range = static_cast<Range>(get_range_string.toInt());
        result = true;
    }

    return result;
}

//------------------------------------------------------------------------------
