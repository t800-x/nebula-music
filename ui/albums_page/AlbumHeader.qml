import QtQuick
import QtQuick.Controls
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import "../utils"

Rectangle {
    anchors.top: parent.top
    property string text: "Albums"
    property var table
    width: parent.width
    height: header.font.pixelSize * 3
    color: Consts.player_color
    id: root

    Label {
        id: header
        anchors.centerIn: parent
        text: root.text
        font.pixelSize: 22
        color: "white"
        font.bold: true
    }

    TextBox {
        height: 32
        width: 200

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 20
        }

        onChanged: table.setFilterText(txt)
    }

    Seperator{
        w: parent.width
        anchors.top: parent.bottom
    }
}
