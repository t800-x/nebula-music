import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Media


Rectangle {
    color: Consts.player_color
    height: Consts.player_height
    width: parent.width
    id: root
    property var blursource
    z: 5

    Rectangle {
        color: "transparent"
        // border.color: "red"
        anchors {
            top: parent.top
            left: parent.left
            right: now_playing.left
            bottom: parent.bottom
        }

        Rectangle {
            height: parent.height
            width: parent.width * 0.75
            color: "transparent"
            // border.color: "red"
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
            }


            Player_controls {
                id: player_controls
                anchors {
                    centerIn: parent
                }

                onPrev_song: {
                    console.log("Prev")
                    MediaPlayer.prev()
                }

                onPlause_song: {
                    console.log("Plause")
                    console.log(MediaPlayer.get_title())
                    MediaPlayer.plause()
                }

                onNext_song: {
                    console.log("Next")
                    MediaPlayer.next()
                }
            }
        }

    }



    Connections {
        target: MediaPlayer
        function onPlayer_state_changed() {
            console.log("Playback state: " + MediaPlayer.state())
            if (MediaPlayer.state() === 2){
                player_controls.paused = true
            }else if ((MediaPlayer.state() === 1)) {
                player_controls.paused = false
            }
        }
    }

    Now_playing {
        id: now_playing
        height: 50
        width: 430

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }


}
