//------------------------------------------------------------------------------

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStringListModel>

#include "error.h"
#include "include/multimeter.h"
#include "include/multimeter_model.h"

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Multimeter multimeter_;

    if (Error::success(multimeter_.init()))
    {
        qmlRegisterType<Multimeter_model>("Multimeter", 1, 0, "Multimeter_model");
        qmlRegisterUncreatableType<Multimeter>("Multimeter", 1, 0, "Multimeter",
                                         QStringLiteral("MultimeterList should not be created in QML"));

        engine.rootContext()->setContextProperty(QStringLiteral("multimeter_"), &multimeter_);
        engine.rootContext()->setContextProperty(QStringLiteral("range_list"), Channel::range_list);
        engine.rootContext()->setContextProperty(QStringLiteral("available_channel_list"), &multimeter_.available_channel_list_model());
        engine.rootContext()->setContextProperty(QStringLiteral("busy_channel_list"), &multimeter_.busy_channel_list_model());

        engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));
    }
    else
        engine.load(QUrl(QLatin1String("qrc:/qml/connect_failed.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

//------------------------------------------------------------------------------
