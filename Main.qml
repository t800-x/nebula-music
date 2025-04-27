import QtQuick
import "Consts.js" as Consts
import QtQuick.Controls.Material
import QtQuick.Layouts
import Nebula.Media

ApplicationWindow {

    Component.onCompleted: {
        MediaPlayer.init()
    }

    FontLoader {
        id: cupertino
        source: "icons/CupertinoIcons.ttf"

        Component.onCompleted: {
            console.log(cupertino.name)
        }
    }

    id: root
    width: 1555
    height: 820
    visible: true
    title: "Nebula Music Player"
    color: Consts.main_bg_color
    Material.theme: Material.system
    Material.accent: Consts.accent

    Rectangle {
        id: nav_container
        anchors.top: parent.top
        anchors.left: parent.left
        color: Consts.nav_color
        width: parent.width * 0.16077
        height: parent.height

        Navbar{
            id: nav
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
        }
    }

    Seperator {
        anchors.left: nav_container.right
        id: nav_br
        h: parent.height
        clr: Qt.darker(Consts.nav_color)
    }

    Player {
        id: player
        anchors {
            top: parent.top
            left: nav_br.right
            right: parent.right
        }

        Seperator {
            id: player_br
            anchors.top: player.bottom
            width: parent.width
            clr: Qt.lighter(Consts.player_color)
        }
    }



    Songs_page {
        anchors {
            top: player.bottom
            left: nav_br.right
            right: parent.right
        }
    }
}
