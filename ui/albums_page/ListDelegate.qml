import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Nebula.Media
import "../utils"

Rectangle {
    id: root
    property string title: "unknown"
    property string time: "00:00"
    property string path
    property int num
    property int idx
    property var table
    property bool hovered: false
    signal clicked()

    radius: 7
    color: idx % 2 === 0 ? Qt.rgba(1, 1, 1, 0.025) : "transparent"
    height: 45

    Rectangle {
        anchors.fill: parent
        visible: root.hovered
        color: Qt.rgba(1, 1, 1, 0.025)
        radius: root.radius
    }

    HoverHandler {
        // whenever the mouse enters *anywhere* in root (including children),
        // hovered becomes true; when it leaves all of root, hovered becomes false.
        onHoveredChanged: root.hovered = hovered
    }


    Row {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        spacing: 10
        Item {
            anchors.verticalCenter: parent.verticalCenter
            height: 25
            width: 25

            Label {
                id: label
                anchors.centerIn: parent
                text: root.num
                color: Qt.rgba(1, 1, 1, 0.5)
                font.pointSize: 11
                visible: !root.hovered
            }

            NormalButton {
                id: btn

                anchors.centerIn: label
                icon: "\uF488"
                width: 30
                height: 30
                onClicked: {
                    MediaPlayer.play(root.table.toSongModel(root), root.idx)
                }

                visible: root.hovered
            }
        }

        Label {
            text: root.title
            color: "white"
            font.pointSize: 11.5
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
