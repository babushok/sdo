//------------------------------------------------------------------------------

#include "include/multimeter_model.h"

//------------------------------------------------------------------------------

Multimeter_model::Multimeter_model(QObject *parent)
    : QAbstractListModel(parent), multimeter_(nullptr)
{
}

//------------------------------------------------------------------------------

int Multimeter_model::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !multimeter_)
        return 0;

    return multimeter_->channels().size();
}

//------------------------------------------------------------------------------

QVariant Multimeter_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !multimeter_)
        return QVariant();

    const Channel channel = multimeter_->channels().at(index.row());

    switch (role) {

    case static_cast<int>(Roles::channel_number):
        return QVariant(channel.number);

    case static_cast<int>(Roles::measure_state):
        return QVariant(channel.measure_state);

    case static_cast<int>(Roles::voltage):
        return QVariant(channel.voltage);

    case static_cast<int>(Roles::range):
        return QVariant(Channel::range_list[static_cast<int>(channel.range)]);

    case static_cast<int>(Roles::state):
        return QVariant(channel.state);
    }
    return QVariant();
}

//------------------------------------------------------------------------------

bool Multimeter_model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!multimeter_)
        return false;

    Channel channel = multimeter_->channels().at(index.row());

    switch (role) {

    case static_cast<int>(Roles::channel_number):
        channel.number = value.toInt();
        break;

    case static_cast<int>(Roles::measure_state):
        channel.measure_state = value.toBool();
        break;

    case static_cast<int>(Roles::voltage):
        channel.voltage = value.toString();
        break;

    case static_cast<int>(Roles::range):
        channel.range = static_cast<Channel::Range>(value.toInt());
        break;

    case static_cast<int>(Roles::state):
        channel.state = value.toString();
        break;
    }

    if (multimeter_->set_channel_at(index.row(), channel)) {
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

Qt::ItemFlags Multimeter_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

//------------------------------------------------------------------------------

QHash<int, QByteArray> Multimeter_model::roleNames() const
{
    QHash<int, QByteArray> names;
    names[static_cast<int>(Roles::channel_number)]  = "channel_number";
    names[static_cast<int>(Roles::measure_state)]   = "measure_state";
    names[static_cast<int>(Roles::voltage)]         = "voltage";
    names[static_cast<int>(Roles::range)]           = "range";
    names[static_cast<int>(Roles::state)]           = "state";

    return names;
}

//------------------------------------------------------------------------------

Multimeter *Multimeter_model::multimeter() const
{
    return multimeter_;
}

//------------------------------------------------------------------------------

void Multimeter_model::set_multimeter(Multimeter *multimeter)
{
    beginResetModel();

    if(multimeter_)
        multimeter_->disconnect(this);

    multimeter_ = multimeter;

    if(multimeter_){
        connect(multimeter_, &Multimeter::pre_channel_append, this, [=](int index){
            beginInsertRows(QModelIndex(), index, index);
        });
        connect(multimeter_, &Multimeter::post_channel_append, this, [=](){
            endInsertRows();
        });
        connect(multimeter_, &Multimeter::pre_channel_removed, this, [=](int index){
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(multimeter_, &Multimeter::post_channel_removed, this, [=](){
            endRemoveRows();
        });
        connect(multimeter_, &Multimeter::data_changed, this, [=](){
            dataChanged(index(0), index(multimeter_->channels().size()-1));
        });
    }

    endResetModel();
}

//------------------------------------------------------------------------------
