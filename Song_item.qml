import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media
import QtQuick.Effects

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
            color: "transparent"

            property bool hovered: false
            property bool pressed: false

            Image {
                id: cover
                source: img_source
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                visible: false
            }

            // MultiEffect mask for rounded corners
            MultiEffect {
                anchors.fill: cover
                source: cover

                maskEnabled: true                           // Turn on masking :contentReference[oaicite:4]{index=4}
                maskSource: roundedMask                      // Use our rectangle as mask :contentReference[oaicite:5]{index=5}

                // Anti-aliasing tweaks (optional but recommended)
                maskThresholdMin: 0.5                        // Sharpness threshold :contentReference[oaicite:6]{index=6}
                maskSpreadAtMin: 1.0                         // Edge softness :contentReference[oaicite:7]{index=7}
            }

            // Mask definition
            Item {
                id: roundedMask
                width: cover.width
                height: cover.height
                visible: false                               // Hide mask itself

                layer.enabled: true                          // Required for maskSource :contentReference[oaicite:8]{index=8}
                layer.smooth: true                           // Smooth out edges :contentReference[oaicite:9]{index=9}

                Rectangle {
                    width: parent.width
                    height: parent.height
                    radius: 5                               // Desired corner radius :contentReference[oaicite:10]{index=10}
                    color: "#ff000000"                       // Only alpha channel is used for masking
                }
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

