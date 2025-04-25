import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

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

        TextField {
            placeholderText: focus || text? "" : "Search"
            id: search_field
            width: songs_btn.width * 0.85
            height: songs_btn.height
            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                radius: 4
                color: Qt.lighter(Consts.nav_color)// ← dark gray when active, darker when inactive
                border.color: search_field.activeFocus ? Consts.accent : Qt.darker("white")
                border.width: 1
            }
        }

        Item {
            width: 1
            height: 40
        }

        Navbar_button {
            id: songs_btn
            text: "Songs"
            iconSource: "icons/music.svg"
            visibility: true
        }
    }
}

