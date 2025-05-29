import QtQuick
import QtQuick.Controls.Material
import Nebula.Database
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import "../utils"

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

    TextBox {
        id: search
        height: 40
        width: 100

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: 25
        }

        onChanged: {
            console.log("Searching for: " + search.text)
            Keeper.setFilter(search.txt)
        }
    }
}
