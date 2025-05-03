import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

Rectangle {
    id: root
    height: header.font.pixelSize * 3
    color: Consts.player_color

    Label {
        id: header
        text: "Songs"
        anchors.centerIn: parent
        font.bold: true
        font.pixelSize: 22
        color: "white"
    }
}
