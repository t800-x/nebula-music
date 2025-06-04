import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import Nebula.Events
import Nebula.Database
import Nebula.Media

Rectangle {
    id: root
    property string title: "unknown"
    property string artist: "unknown"
    property int albumId: -1
    property string cover_img: Keeper.getAlbumCover(albumId)
    color: "transparent"

    Column {
        anchors.fill: parent
        spacing: 3



        Rectangle {
            id: cover_container
            property bool hovered: false
            height: 150
            width: 150
            color: "transparent"
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: alphaLayer
                anchors.fill: cover
                color: cover_container.hovered ? Qt.rgba(1, 1, 1, 0.03) : "transparent"
                radius: 5
                z: 68

                AlButton {
                    height: 30
                    width: 30
                    visible: cover_container.hovered

                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        margins: 5
                    }

                    onClicked: MediaPlayer.play(Keeper.get_album_songs(root.albumId, root).toSongModel(root), 0)

                    glyph: "\uF488"
                }

                //Option Menu
                // AlButton {
                //     height: 30
                //     width: 30
                //     visible: cover_container.hovered

                //     anchors {
                //         bottom: parent.bottom
                //         right: parent.right
                //         margins: 5
                //     }

                //     glyph: "\uF46A"
                // }
            }

            HoverHandler {
                onHoveredChanged: cover_container.hovered = hovered
            }

            MouseArea {
                anchors.fill: parent

                onClicked: EventBus.emitShowAlbum(root.albumId, root.cover_img, root.title, root.artist)
            }

            Image {
                id: cover
                source: root.cover_img
                visible: false
                mipmap: true

                anchors.fill: parent
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

                visible: true
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
        }


        Label {
            text: title
            font.pointSize: 12
            color: "white"
            anchors.left: cover_container.left
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            width: cover_container.width
        }

        Label {
            text: artist
            font.pointSize: 10
            color: Qt.rgba(1, 1, 1, 0.5)
            anchors.left: cover_container.left
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            width: cover_container.width
        }
    }
}
