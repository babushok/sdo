import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    x: Math.round(screen.width/2 - width/2)
    y: Math.round(screen.height/2 - height/2)
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height
    color: "black"

    title: qsTr("QML Client")

    flags: Qt.Window

    Multimeter {
        anchors.centerIn: parent
    }
}
