import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    id: root
    property string icon: ""
    property string text: ""
    property bool hovered: false
    signal clicked()

    color: Consts.accent
    height: 35
    width: 115
    radius: 6

    Rectangle {
        id: alphaLayer
        color: Qt.rgba(1, 1, 1, 0.25)
        anchors.fill: parent
        visible: root.hovered
        radius: parent.radius
    }

    Row {
        id: container
        anchors.centerIn: parent
        spacing: 5

        Label {
            text: root.icon
            font.family: "CupertinoIcons"
            font.pointSize: 16
            color: "white"
        }

        Label {
            text: root.text
            font.pointSize: 12
            color: "white"
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onClicked: root.clicked()
    }
}
