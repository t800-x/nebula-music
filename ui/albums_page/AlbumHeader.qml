import QtQuick
import QtQuick.Controls
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import "../utils"

Rectangle {
    anchors.top: parent.top
    width: parent.width
    height: header.font.pixelSize * 3
    color: Consts.player_color

    Label {
        id: header
        anchors.centerIn: parent
        text: "Albums"
        font.pixelSize: 22
        color: "white"
        font.bold: true
    }

    Seperator{
        w: parent.width
        anchors.top: parent.bottom
    }
}
