import QtQuick
import "Consts.js" as Consts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Nebula.Events

import "ui/nav"
import "ui/utils"
import "ui/player"
import "ui/queue"
import "ui/songs_page"

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
            width: parent.width * 0.15
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
            blursource: allContent
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

        StackView {
            id: canvas

            anchors {
                top: player.bottom
                left: nav_br.right
                bottom: parent.bottom
                right: sidepanel.left
            }

            Component.onCompleted: {
                canvas.push("ui/songs_page/Songs_page.qml")
            }
        }

        StackView {
            id: sidepanel

            width: 0 /*empty ? 0 : parent.width * 0.17*/
            height: parent.height
            anchors {
                top: player.bottom
                right: parent.right
            }

            // Component.onCompleted: {
            //     sidepanel.push("ui/queue/Queue.qml")
            // }

            Behavior on width {
                NumberAnimation {
                    easing.type: easing.InOutQuad
                    duration: 150
                }
            }
        }

        //Just for the sidepane, ik its dirty
        Connections {
            target: EventBus

            function onQueueButtonClicked() {
                if (sidepanel.currentItem?.name === "Queue" && sidepanel.width > 0) {
                    console.log(1)
                    sidepanel.width = 0
                    EventBus.setCurrentPane("");
                }else if (sidepanel.currentItem?.name === "Queue" && sidepanel.width === 0){
                    sidepanel.width = allContent.width * 0.17
                    console.log(2)
                    EventBus.setCurrentPane("Queue")
                }else if (sidepanel.currentItem?.name !== "Queue" && (!sidepanel.empty)) {
                    sidepanel.replace("ui/queue/Queue.qml")
                    sidepanel.width = allContent.width * 0.17
                    EventBus.setCurrentPane("Queue")
                    console.log(3)
                }else{
                    sidepanel.width = allContent.width * 0.17
                    sidepanel.push("ui/queue/Queue.qml")
                    EventBus.setCurrentPane("Queue")
                    console.log(4)
                }
            }

            function onLyricsButtonClicked() {
                if (sidepanel.currentItem?.name === "LyricsPane" && sidepanel.width > 0) {
                    console.log(1)
                    sidepanel.width = 0
                    EventBus.setCurrentPane("")
                }else if (sidepanel.currentItem?.name === "LyricsPane" && sidepanel.width === 0){
                    sidepanel.width = allContent.width * 0.17
                    EventBus.lyricsViewChanged()
                    console.log(2)
                    EventBus.setCurrentPane("LyricsPane")
                }else if (sidepanel.currentItem?.name !== "LyricsPane" && (!sidepanel.empty)) {
                    sidepanel.replace("ui/lyrics/LyricsPane.qml")
                    sidepanel.width = allContent.width * 0.17
                    EventBus.lyricsViewChanged()
                    console.log(3)
                    EventBus.setCurrentPane("LyricsPane")
                }else{
                    sidepanel.width = allContent.width * 0.17
                    sidepanel.push("ui/lyrics/LyricsPane.qml")
                    EventBus.lyricsViewChanged()
                    console.log(4)
                    EventBus.setCurrentPane("LyricsPane")
                }
            }
        }
    }
}
