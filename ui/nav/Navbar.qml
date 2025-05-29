import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import "../utils"

Rectangle {

    height: parent.height
    width: parent.width
    color: Consts.nav_color

    Column {
        id: nav_column
        spacing: 10
        width: parent.width
        height: parent.height

        anchors {
            centerIn: parent
            top: parent.top
        }

        TextBox {
            placeholderText: focus || text? "" : "Search"
            id: search_field
            width: songs_btn.width * 0.85
            height: songs_btn.height
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            width: 1
            height: 40
        }

        Navbar_button {
            id: songs_btn
            text: "Songs"
            // iconSource: "icons/music.svg"
            visibility: true
        }
    }
}

