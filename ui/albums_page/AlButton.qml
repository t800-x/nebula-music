import QtQuick
import QtQuick.Controls
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    radius: 1000
    property string glyph
    id: root
    property bool hovered: false
    color: hovered ? Consts.accent : "transparent"
    signal clicked()

    Label {
        text: root.glyph
        color: "white"
        font.family: "CupertinoIcons"
        font.pointSize: 14
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            root.visible = true
            root.hovered = true
        }
        onExited: root.hovered = false
        onClicked: root.clicked()
    }
}
