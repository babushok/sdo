//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <QAbstractListModel>

#include "multimeter.h"

//------------------------------------------------------------------------------

class Multimeter_model : public QAbstractListModel
{

    Q_OBJECT

    Q_PROPERTY(Multimeter *multimeter READ multimeter WRITE set_multimeter)

//------------------------------------------------------------------------------

public:

    explicit Multimeter_model(QObject *parent = nullptr);

    enum class Roles{
        channel_number = Qt::UserRole+1,
        measure_state,
        voltage,
        range,
        state
    };

//------------------------------------------------------------------------------

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

//------------------------------------------------------------------------------

    Multimeter *multimeter() const;

    void set_multimeter(Multimeter *multimeter);

//------------------------------------------------------------------------------

private:

    Multimeter *multimeter_;

//------------------------------------------------------------------------------

}; //End of class Multimeter_model

//------------------------------------------------------------------------------
