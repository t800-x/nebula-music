import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Media
import QtQuick.Effects
import Nebula.Database

Rectangle {
    id: root
    color: "#393939"
    // border.color: "red"
    radius: 5

    Rectangle {
        id: cover_container
        color: "transparent"
        height: parent.height
        width: height
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            right: data_container.left
        }

        Image {
            id: cover
            source: ""
            visible: false
            mipmap: true

            anchors.fill: parent
        }

        // MultiEffect mask for rounded corners
        MultiEffect {
            id: cover_render

            anchors.fill: cover
            source: cover

            maskEnabled: true
            maskSource: roundedMask

            // Anti-aliasing tweaks (optional but recommended)
            maskThresholdMin: 0.5
            maskSpreadAtMin: 1.0

            visible: false
        }

        // Mask definition: only left corners rounded
        Item {
            id: roundedMask
            width: cover.width
            height: cover.height
            visible: false

            layer.enabled: true
            layer.smooth: true

            Canvas {
                id: maskCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();

                    var w = width;
                    var h = height;
                    var r = 5; // corner radius

                    // Clear previous
                    ctx.clearRect(0, 0, w, h);

                    // Draw path with only left corners rounded
                    ctx.beginPath();
                    ctx.moveTo(r, 0);
                    ctx.lineTo(w, 0);
                    ctx.lineTo(w, h);
                    ctx.lineTo(r, h);
                    // bottom‐left corner:
                    ctx.quadraticCurveTo(0, h, 0, h - r);
                    ctx.lineTo(0, r);
                    // top‐left corner:
                    ctx.quadraticCurveTo(0, 0, r, 0);
                    ctx.closePath();

                    // Fill with opaque black (only alpha matters for mask)
                    ctx.fillStyle = "#ff000000";
                    ctx.fill();
                }

                // Redraw whenever size changes
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
            }
        }
    }


    Rectangle {
        id: data_container
        color: "transparent"
        // border.color: "red"
        anchors {
            left: cover_container.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }


        Label {
            id: title
            text: ""
            color: "white"
            font.pixelSize: 13
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
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
            anchors.horizontalCenter: parent.horizontalCenter
        }



        Slider {
            id: seekbar
            width: root.width - cover.width - 10
            height: 10 // Explicit height to reduce space
            anchors {
                bottom: parent.bottom
                bottomMargin: 1 // Adds space from the bottom
                horizontalCenter: parent.horizontalCenter
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
                cover.source = Keeper.getCover(MediaPlayer.getCurrentSongId())
                console.log(cover.source)
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
