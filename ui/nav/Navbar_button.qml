import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Events

Rectangle {
    id: root
    property alias text: label.text
    property string glyph
    property bool active: false
    required property string name
    signal clicked

    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width * 0.9
    height: label.font.pixelSize * 2
    radius: 8

    property bool hovered: false
    property bool pressed: false

    color: {
        // if (!active) return "transparent"
        if (active && hovered) return Qt.rgba(1, 1, 1, 0.3)
        else if (active && !hovered) return Qt.rgba(1, 1, 1, 0.1)
        else if(!active && hovered)return Qt.rgba(1, 1, 1, 0.1)
        else return "transparent"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onPressed: root.pressed = true
        onReleased: {
            root.pressed = false
            root.clicked()   //Expose this signal
        }
    }

    Row {
        anchors.top: parent.top
        anchors.left: parent.left
        leftPadding: 12
        spacing: 10

        Label {
            id: icon
            text: glyph
            font.family: "CupertinoIcons"
            color: "white"
            font.pointSize: 14
            y: +3
        }

        Label {
            id: label
            font.pixelSize: 14
            color: "white"
            y: +3
        }
    }

    Connections{
        target: EventBus

        function onNavButtonClicked(btn) {
            if (root.name === btn) active = true
            else active = false
        }
    }
}
