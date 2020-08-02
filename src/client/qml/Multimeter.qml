import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import Multimeter 1.0

ColumnLayout {

    Frame {

        Dialog {
            id: recv_failed_dialog
            x: Math.round(parent.width/2 - parent.width/4)
            y: Math.round(parent.height/2 - parent.height/4)
            width: Math.round(parent.width/2)
            modal: true
            focus: true

            standardButtons: Dialog.Abort

            onRejected: Qt.quit()

            contentItem: Text {
                Layout.fillWidth: true
                font.pixelSize: 24
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                smooth: true
                color: "#E75218"
                wrapMode: Text.WordWrap
                text: "<b>Error: </b>get data<br>Connection failed<br>Program aborted"
            }
        }

        ListView {
            spacing: 0
            implicitWidth: 750
            implicitHeight: 500
            clip: true
            anchors.fill: parent

            model: Multimeter_model {
                multimeter: multimeter_
            }

            delegate: RowLayout{

                width:  parent.width
                spacing: 0

                Rectangle {
                    id: rect_number_data
                    width: 120
                    height: 50
                    Layout.alignment : Qt.AlignLeft
                    color: "#614996"

                    Text {
                        text: model.channel_number
                        Layout.fillWidth: true
                        font.pixelSize: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        smooth: true
                        color: "white"
                    }
                }

                Rectangle {
                    id: rect_measure_state_data
                    width: 120
                    height: 50
                    Layout.alignment : Qt.AlignLeft
                    color: "#614996"
                    Switch {
                        checked: model.measure_state
                        onClicked: model.measure_state = checked
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                Rectangle {
                    id: rect_voltage_data
                    width: 230
                    height: 50
                    Layout.alignment : Qt.AlignLeft
                    color: "#614996"
                    Text {
                        text: model.voltage
                        Layout.fillWidth: true
                        font.pixelSize: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        smooth: true
                        color: "white"
                    }
                }

                Rectangle {
                    id: rect_status_data
                    width: 120
                    height: 50
                    Layout.alignment : Qt.AlignLeft
                    color: multimeter_.is_measure_state(model.state) ? "#97B510" : multimeter_.is_busy_state(model.state) ? "#C08E2D" : multimeter_.is_error_state(model.state)? "#E75218" : "#614996"

                    Text {
                        text: model.state
                        Layout.fillWidth: true
                        font.pixelSize: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        smooth: true
                        color: "white"
                    }
                }

                Rectangle {
                    id: rect_range_data
                    width: 190
                    height: 50
                    Layout.alignment : Qt.AlignLeft
                    color: "#614996"

                    Text {
                        id: text_range_data
                        text: model.range
                        Layout.fillWidth: true
                        font.pixelSize: 24
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        smooth: true
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: range_dialog.open()
                    }

                    Dialog {
                        id: range_dialog
                        x: -10
                        y: -60
                        width: 200
                        modal: true
                        focus: true
                        title: "Choose range"

                        standardButtons: Dialog.Ok | Dialog.Cancel
                        onAccepted: {
                            model.range = range_box.currentIndex
                            range_dialog.close()
                        }
                        onRejected: {
                            range_box.currentIndex = range_box.range_index
                            range_dialog.close()
                        }

                        contentItem: ColumnLayout {
                            id: range_column
                            ComboBox {
                                id: range_box
                                property int range_index: -1
                                model: range_list
                                Component.onCompleted: {
                                    range_index = find(text_range_data.text, Qt.MatchFixedString)
                                    if (range_index !== -1)
                                        currentIndex = range_index
                                }
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }

            headerPositioning: ListView.OverlayHeader

            header: bannercomponent

            footer: Rectangle {
                width: parent.width; height: 30;
                gradient: Gradient {
                    GradientStop { position: 0.2; color: "#614996"}
                    GradientStop { position: 1.0; color: "#8B76BA"}
                }
            }
        }
    }

    Component {
        id: bannercomponent

        RowLayout{
            width:  parent.width
            spacing: 0
            z : 2

            Rectangle {
                id: rect_number_header
                width: 120
                height: 50
                Layout.alignment : Qt.AlignLeft
                gradient: header_gradient
                Text {
                    anchors.centerIn: parent
                    color: "#97B510"
                    text: "Channel"
                    font.pixelSize: 26
                }
            }

            Rectangle {
                id: rect_measure_state_header
                width: 120
                height: 50
                Layout.alignment : Qt.AlignLeft
                gradient: header_gradient
                Text {
                    anchors.centerIn: parent
                    color: "#97B510"
                    text: "Measure"
                    font.pixelSize: 26
                }
            }

            Rectangle {
                id: rect_voltage_header
                width: 230
                height: 50
                Layout.alignment : Qt.AlignRight
                gradient: header_gradient
                Text {
                    anchors.centerIn: parent
                    color: "#97B510"
                    text: "Voltage"
                    font.pixelSize: 26
                }
            }

            Rectangle {
                id: rect_status_header
                width: 120
                height: 50
                Layout.alignment : Qt.AlignRight
                gradient: header_gradient
                Text {
                    anchors.centerIn: parent
                    color: "#97B510"
                    text: "Status"
                    font.pixelSize: 26
                }
            }

            Rectangle {
                id: rect_range_header
                width: 190
                height: 50
                Layout.alignment : Qt.AlignRight
                gradient: header_gradient
                Text {
                    anchors.centerIn: parent
                    color: "#97B510"
                    text: "Range"
                    font.pixelSize: 26
                }
            }
        }
    }

    Gradient {
        id: header_gradient
        GradientStop { position: 0.0; color: "#8B76BA"}
        GradientStop { position: 0.66; color: "#614996"}
    }

    Gradient {
        id: data_gradient
        GradientStop { position: 0.1; color: "#614996"}
        GradientStop { position: 0.5; color: "#69529D"}
        GradientStop { position: 0.9; color: "#614996"}
    }

    Dialog {
        id: add_channel_dialog
        x: 20
        y: 370
        width: 200
        modal: true
        focus: true
        title: "Append channel"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            add_channel_dialog.close()
            multimeter_.append_channel(add_channel_box.displayText)
            add_chanel_button.enabled = multimeter_.is_available_channels()
            remove_channel_button.enabled = multimeter_.is_busy_channels()
            get_data_button.enabled = multimeter_.is_busy_channels()
            auto_refresh_box.enabled = multimeter_.is_busy_channels()
            add_channel_box.currentIndex = 0
        }
        onRejected: {
            add_channel_dialog.close()
        }

        contentItem: ColumnLayout {
            ComboBox {
                id: add_channel_box
                textRole: "display"
                model: available_channel_list
                Component.onCompleted: currentIndex = 0
                Layout.fillWidth: true
            }
        }
    }

    Dialog {
        id: remove_channel_dialog
        x: 220
        y: 370
        width: 200
        modal: true
        focus: true
        title: "Remove channel"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            remove_channel_dialog.close()
            multimeter_.remove_channel(remove_channel_box.displayText)
            add_chanel_button.enabled = multimeter_.is_available_channels()
            remove_channel_button.enabled = multimeter_.is_busy_channels()
            get_data_button.enabled = multimeter_.is_busy_channels()
            auto_refresh_box.enabled = get_data_button.enabled
            if(!multimeter_.is_busy_channels())
            {
                auto_refresh_box.checked = false
                get_data_timer.running = auto_refresh_box.checked
            }
            remove_channel_box.currentIndex = 0
        }
        onRejected: {
            remove_channel_dialog.close()
        }

        contentItem: ColumnLayout {
            ComboBox {
                id: remove_channel_box
                textRole: "display"
                model: busy_channel_list
                Component.onCompleted: currentIndex = 0
                Layout.fillWidth: true
            }
        }
    }

    RowLayout{

        ColorButton {
            id: add_chanel_button
            text : qsTr("Add channel")
            font.pixelSize: 18
            enabled: multimeter_.is_available_channels()
            onClicked: if (multimeter_.is_available_channels()) add_channel_dialog.open()
        }

        ColorButton {
            id: remove_channel_button
            text : qsTr("Remove channel")
            font.pixelSize: 18
            enabled: multimeter_.is_busy_channels()
            onClicked: if (multimeter_.is_busy_channels()) remove_channel_dialog.open()
        }

        Rectangle {
            implicitWidth: 105
            implicitHeight: 40
            Layout.leftMargin: 150
            color: "black"

            CheckBox{
                id: auto_refresh_box
                anchors.fill: parent
                enabled: multimeter_.is_busy_channels()
                contentItem: Text {
                    anchors.fill: parent
                    text: auto_refresh_box.text
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    color: auto_refresh_box.checked ? "#97B510" : "white"
                }
                text: qsTr("Refresh")
                onClicked: {
                    get_data_timer.running = auto_refresh_box.checked
                    get_data_button.enabled = !auto_refresh_box.checked
                }
            }
        }

        Timer {
            id: get_data_timer
            running: false;
            repeat: true
            onTriggered: if(!multimeter_.get_data()) recv_failed_dialog.open()
        }

        ColorButton2 {
            id: get_data_button
            Layout.leftMargin: 10
            text : qsTr("Get data")
            font.pixelSize: 20
            enabled: multimeter_.is_busy_channels()
            Layout.fillWidth: true
            onClicked: if(!multimeter_.get_data()) recv_failed_dialog.open()
        }
    }
}
