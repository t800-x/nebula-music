import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Media

Rectangle {
    width: row.implicitWidth
    height: parent.height
    color: "transparent"
    property bool paused: false
    signal prev_song
    signal plause_song
    signal next_song
    id: root
    // border.color: "red" <-- Debug

    Row {
        anchors.centerIn: parent
        id: row

        Rectangle {
            property bool hovered: false
            property bool pressed: false
            id: prev
            width: prev_btn.implicitWidth * 1.75
            height: prev_btn.implicitHeight * 1.75
            color: hovered ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
            anchors.verticalCenter: parent.verticalCenter
            radius: 6
            Label {
                id: prev_btn
                text: ""
                font.family: "CupertinoIcons"
                color: prev.pressed ? "#787878" : "white"
                font.pointSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.prev_song()
                onEntered: prev.hovered = true
                onExited: prev.hovered = false
                onPressed: prev.pressed = true
                onReleased: prev.pressed = false
            }
        }

        Rectangle {
            id: plause
            radius: 6
            width: prev_btn.implicitWidth * 1.75
            height: prev_btn.implicitHeight * 1.75
            anchors.verticalCenter: parent.verticalCenter
            property bool pressed: false
            property bool hovered: false
            color: hovered ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
            Label {
                id: plause_btn
                text: {
                    if (paused) {
                        return ""
                    } else {
                        return ""
                    }
                }

                font.family: "CupertinoIcons"
                color: plause.pressed ? "#787878" : "white"
                font.pointSize: 25
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    root.plause_song()
                }
                onEntered: plause.hovered = true
                onExited: plause.hovered = false
                onPressed: plause.pressed = true
                onReleased: plause.pressed = false
            }
        }

        Rectangle {
            id: next
            width: next_btn.implicitWidth * 1.75
            height: next_btn.implicitHeight * 1.75
            anchors.verticalCenter: parent.verticalCenter
            radius: 6
            property bool hovered: false
            property bool pressed: false
            color: hovered ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
            Label {
                id: next_btn
                text: ""
                font.family: "CupertinoIcons"
                color: next.pressed ? "#787878" : "white"
                font.pointSize: 20
                anchors.centerIn: parent
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    root.next_song()
                }
                onEntered: next.hovered = true
                onExited: next.hovered = false
                onPressed: next.pressed = true
                onReleased: next.pressed = false
            }
        }
    }
}
