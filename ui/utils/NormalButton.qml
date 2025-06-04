//NormalButton.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    property string icon: ""
    property bool hovered: false
    radius: 5
    color: "transparent"
    id: root
    signal clicked()

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(1, 1, 1, 0.07)
        visible: root.hovered
        radius: root.radius
    }

    Label {
        text: parent.icon
        font.family: "CupertinoIcons"
        font.pointSize: 14
        color: Consts.accent
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            root.hovered = true
        }
        onExited: {
            root.hovered = false
        }
        onClicked: {
            root.clicked()
        }
    }
}
