import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    required property string text
    signal clicked()

    property bool hovered: false
    property bool pressed: false

    height: clear_label.implicitHeight * 1.25
    width: clear_label.implicitWidth * 1.25
    color: {
        if (pressed) return Qt.rgba(1, 1, 1, 0.2)
        else if (hovered) return Qt.rgba(1, 1, 1, 0.1)
        else return Qt.rgba(1, 1, 1, 0.075)
    }

    radius: 5

    Label {
        id: clear_label
        text: root.text
        color: "white"
        anchors.centerIn: parent
        font.pointSize: 10
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
            root.clicked()
        }
    }
}
