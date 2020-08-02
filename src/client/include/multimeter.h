//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <QObject>
#include <QVector>
#include <QMap>
#include <QStringListModel>

#include "client.h"

//------------------------------------------------------------------------------

struct Channel
{
    enum class State {
        error_state, idle_state, measure_state, busy_state
    };

    enum class Range {
        r_1000uv, r_1000mv, r_1000v, r_1000kv
    };

    enum class Command {
        start_measure, stop_measure, set_range, get_status, get_result, channel, range, ok, fail
    };

//------------------------------------------------------------------------------

    static bool request_success(const result_t &reply);
    std::vector<std::string> parse_reply(const result_t &reply);
    QString command_string(Channel::Command command);
    QString command_string(Channel::Command command, Channel::Command arg, int argv);

    bool set_measure_state(Client &client);
    bool get_voltage(Client &client);
    bool get_state(Client &client);
    bool set_range(Client &client);

//------------------------------------------------------------------------------

    int number;
    bool measure_state;
    QString voltage;
    Range range;
    QString state;

//------------------------------------------------------------------------------

    static inline QString voltage_str = "0.0";
    static inline Range default_range = Range::r_1000v;

    static const inline QMap<Channel::State, QString> state_strings =
    {{State::error_state,   QStringLiteral("error")},
     {State::idle_state,    QStringLiteral("idle")},
     {State::measure_state, QStringLiteral("measure")},
     {State::busy_state,    QStringLiteral("busy")}
    };

    static const inline QMap<Channel::Range, QString> range_strings =
    {{Range::r_1000uv, QStringLiteral("1000 µV")},
     {Range::r_1000mv, QStringLiteral("1000 mV")},
     {Range::r_1000v,  QStringLiteral("1000 V")},
     {Range::r_1000kv, QStringLiteral("1000 kV")}
    };

    static const inline QMap<Channel::Command, QString> command_strings =
    {{Command::start_measure, QStringLiteral("start_measure")},
     {Command::stop_measure,  QStringLiteral("stop_measure")},
     {Command::set_range, QStringLiteral("set_range")},
     {Command::get_status, QStringLiteral("get_status")},
     {Command::get_result, QStringLiteral("get_result")},
     {Command::channel, QStringLiteral("channel")},
     {Command::range, QStringLiteral("range")},
     {Command::ok, QStringLiteral("ok")},
     {Command::fail, QStringLiteral("fail")}
    };

    static const inline QStringList range_list =
    {range_strings[Range::r_1000uv],
     range_strings[Range::r_1000mv],
     range_strings[Range::r_1000v],
     range_strings[Range::r_1000kv]
    };

//------------------------------------------------------------------------------

}; //End of class Channel

//------------------------------------------------------------------------------

class Multimeter : public QObject
{
    enum class Settings {company, product, channels, number, measure_state, voltage, state, range};

//------------------------------------------------------------------------------

    Q_OBJECT

//------------------------------------------------------------------------------

public:

    Q_INVOKABLE bool is_error_state(QString state);
    Q_INVOKABLE bool is_idle_state(QString state);
    Q_INVOKABLE bool is_measure_state(QString state);
    Q_INVOKABLE bool is_busy_state(QString state);
    Q_INVOKABLE bool is_available_channels();
    Q_INVOKABLE bool is_busy_channels();

    Q_INVOKABLE bool get_data();

//------------------------------------------------------------------------------

    explicit Multimeter(QObject *parent = nullptr) : QObject(parent), client_{"client_qml"} { }
    ~Multimeter();

    Error::type init();

    QVector<Channel> channels() const;

    bool set_channel_at(int index, const Channel &channel);

    QStringListModel& available_channel_list_model() {return available_channel_list_model_;}
    QStringListModel& busy_channel_list_model() {return busy_channel_list_model_;}

//------------------------------------------------------------------------------

signals:

    void pre_channel_append(int index);
    void post_channel_append();

    void pre_channel_removed(int index);
    void post_channel_removed();

    void data_changed();

//------------------------------------------------------------------------------

public slots:

    void append_channel(int channel_number);
    void remove_channel(int channel_number);

//------------------------------------------------------------------------------

private:

    int get_channel_count() const {return channel_count_;}
    void set_channel_count();
    void set_channel_list_model();
    QStringList available_channel_list();
    QStringList busy_channel_list();

    void get_settings();
    void set_settings();

    const int max_channel_count = 32;
    int channel_count_ = 8;

    QStringListModel available_channel_list_model_;
    QStringListModel busy_channel_list_model_;

    QVector<Channel> channels_;

    Client client_;

    static const inline QMap<Multimeter::Settings, QString> settings_strings =
    {{Settings::company,        QStringLiteral("sdo")},
     {Settings::product,        QStringLiteral("client_qml")},
     {Settings::channels,       QStringLiteral("channels")},
     {Settings::number,         QStringLiteral("number")},
     {Settings::measure_state,  QStringLiteral("measure_state")},
     {Settings::voltage,        QStringLiteral("voltage")},
     {Settings::state,          QStringLiteral("state")},
     {Settings::range,          QStringLiteral("range")}
    };

//------------------------------------------------------------------------------

}; //End of class Multimeter

//------------------------------------------------------------------------------
