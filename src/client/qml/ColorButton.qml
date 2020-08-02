import QtQuick 2.7
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

Button {
    id: page0Button0

    contentItem: Text {
        text: page0Button0.text
        font: page0Button0.font
        opacity: enabled ? 1.0 : 0.3
        color: pressed ? "#614996" : "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 90
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: pressed ? "#97B510" : "#8B76BA"
        color: pressed ? "#97B510" : "#8B76BA"
        border.width: 1
    }
}
