import QtQuick
import "Consts.js" as Consts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
// import Nebula.Media

ApplicationWindow {
    id: root
    width: 1555
    height: 820
    visible: true
    title: "Nebula Music Player"
    color: Consts.main_bg_color
    Material.theme: Material.system
    Material.accent: Consts.accent

    FontLoader {
        id: cupertino
        source: "icons/CupertinoIcons.ttf"
    }

    Item {
        id: allContent
        anchors.fill: parent

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
            clr: "#1a1e22"
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
                height: 1
            }
        }

        Songs_page {
            id: songs_page
            anchors {
                top: player.bottom
                left: nav_br.right
                right: queue.left
            }
        }

        Queue {
            id: queue
            width: parent.width * 0.17
            height: parent.height
            anchors {
                top: player.bottom
                right: parent.right
            }
        }
    }
}
