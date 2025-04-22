import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media

Rectangle {
    required property string song_name
    required property string img_source
    property string artist: "Unknown"
    property string album: "Unknown"
    required property string path
    signal clicked

    id: root

    width: parent.width
    height: 52
    radius: 8
    color: "#2a2a2a"


    Row {
        id: song_container
        spacing: 20
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.topMargin: 15

        Rectangle {
            id: cover_container
            height: 40
            width: 40
            y: -10

            property bool hovered: false
            property bool pressed: false

            Image {
                id: cover
                source: img_source
                anchors.fill: parent
            }

            MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: cover_container.hovered = true
                    onExited: cover_container.hovered = false
                    onPressed: cover_container.pressed = true
                    onReleased: {
                        cover_container.pressed = false
                        root.clicked()   //Expose this signal
                        MediaPlayer.set_source(path)
                        MediaPlayer.play()
                    }
                }
        }





        Label {
            text: song_name
            color: "white"
            font.pixelSize: 20
        }

        Label {
            text: album
            color: "white"
            font.pixelSize: 20
        }

        Label {
            text: artist
            color: "white"
            font.pixelSize: 20
        }
    }

}

