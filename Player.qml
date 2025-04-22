import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media


Rectangle {
    color: Consts.player_color
    height: Consts.player_height
    width: parent.width
    z: 5

    Player_controls {
        id: player_controls
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 55
        }

        onPrev_song: {
            console.log("Prev")
        }

        onPlause_song: {
            console.log("Plause")
            console.log(MediaPlayer.get_title())
            MediaPlayer.play()
        }

        onNext_song: {
            console.log("Next")
        }
    }

    Connections {
        target: MediaPlayer
        onPlayer_state_changed: {
            console.log("Playback state: " + MediaPlayer.state())
            if (MediaPlayer.state() === 0) {
                now_playing.text = "Now Playing: "
            }else {
                now_playing.text = "Now Playing: " + MediaPlayer.get_title()
            }
        }
    }

    Label {
        id: now_playing
        anchors {
            left: player_controls.right
            verticalCenter: parent.verticalCenter
            leftMargin: 50
        }

        text: "Now Playing: "
        color: "white"
    }
}
