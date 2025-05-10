import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media
import QtQuick.Effects

Rectangle {
    id: root
    color: "#393939"
    // border.color: "red"
    radius: 5

    Image {
        id: cover
        source: "test_assets/cover.jpg"
        height: 40
        width: 40
        visible: false

        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
    }

    // MultiEffect mask for rounded corners
    MultiEffect {
        id: cover_render

        anchors.fill: cover
        source: cover

        maskEnabled: true                           // Turn on masking :contentReference[oaicite:4]{index=4}
        maskSource: roundedMask                      // Use our rectangle as mask :contentReference[oaicite:5]{index=5}

        // Anti-aliasing tweaks (optional but recommended)
        maskThresholdMin: 0.5                        // Sharpness threshold :contentReference[oaicite:6]{index=6}
        maskSpreadAtMin: 1.0

        visible: false
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

    Label {
        id: title
        text: ""
        color: "white"
        font.pixelSize: 13
        font.bold: true
        x: (root.width - cover.width) / 2
        anchors.top: parent.top
        anchors.topMargin: 5
    }

    Label {
        id: artist
        text: ""
        color: "white"
        font.pixelSize: 12

        anchors.top: title.bottom
        anchors.topMargin: 1
        x: (root.width - cover.width) / 2
    }

    Label {
        id: no_media
        font.family: "CupertinoIcons"
        font.pointSize: 20
        color: "white"
        text: ""
        anchors.centerIn: parent
        visible: true
    }

    Slider {
        id: seekbar
        width: root.width - cover.width - 10
        height: 10 // Explicit height to reduce space
        anchors {
            bottom: parent.bottom
            left: parent.left
            bottomMargin: 1 // Adds space from the bottom
            leftMargin: 47
        }

        // Custom handle
        handle: Rectangle {
            x: seekbar.leftPadding + seekbar.visualPosition * (seekbar.availableWidth - width)
            y: seekbar.topPadding + (seekbar.availableHeight - height) / 2
            implicitWidth: 10 // Smaller width
            implicitHeight: 10 // Smaller height
            radius: 6 // Makes it circular
            color: "transparent"
        }

        from: 0
        to: 69

        onMoved: {
            MediaPlayer.set_position(seekbar.value)
        }

        visible: false
    }

    Connections {
        target: MediaPlayer
        function onPlayer_state_changed() {
            if (MediaPlayer.state() !== 0) {
                seekbar.visible = true
                cover_render.visible = true
                no_media.visible = false
                title.visible = true
                artist.visible = true

                title.text = MediaPlayer.get_title()
                artist.text = MediaPlayer.get_artist()
            }

            if (MediaPlayer.state() === 0) {
                seekbar.visible = false
                cover_render.visible = false
                no_media.visible = true

                title.visible = false
                artist.visible = false
            }
        }

        function onTime_changed() {
            seekbar.from = 0
            seekbar.to = MediaPlayer.get_duration()
            seekbar.value = MediaPlayer.get_position()
        }
    }
}
