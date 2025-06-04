import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    id: root
    signal clicked()
    height: label.implicitHeight * 1.5
    width: label.implicitWidth * 4
    radius: 5
    color: hovered ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
    property bool hovered: false

    Label {
        id: label
        text: "\uF3CF"
        font.family: "CupertinoIcons"
        color: Consts.accent
        font.pointSize: 18
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onClicked: root.clicked()
    }
}
