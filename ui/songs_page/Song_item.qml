import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Media
import QtQuick.Effects

Rectangle {
    required property string song_name
    required property string img_source
    property var all_songs
    property int ind
    property bool alt
    property string artist: "Unknown"
    property string album: "Unknown"
    required property string path

    signal clicked
    signal addQueue
    signal playNext

    radius: 5
    clip: true

    property bool hovered: false
    property bool pressed: false

    id: root

    // width: parent.width
    height: song_name_label.font.pixelSize * 2
    color: {
        var changed_color = "#242424"
        if (hovered) {
            return "#2a2a2a"
        }else if((!hovered) && alt) {
            return changed_color
        }else{
            return Consts.main_bg_color
        }
    }

    /*alt === false ? Consts.main_bg_color : Qt.lighter(Consts.main_bg_color)*/

    Grid {
        id: songGrid
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        anchors.topMargin: 4
        columnSpacing: 20
        rows: 1
        columns: 3
        flow: Grid.LeftToRight

        // Column 1: Song Name (50% width)
        Label {
            id: song_name_label
            width: root.width * 0.33
            text: song_name
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font {
                pixelSize: 14
                bold: true
            }
        }

        // Column 2: Artist (25% width)
        Label {
            width: root.width * 0.33
            text: artist
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 14
        }

        // Column 3: Album (25% width)
        Label {
            width: root.width * 0.33
            text: album
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 14
        }

        Menu {
            id: contextMenu
            MenuItem {
                text: "Play Next"
                onTriggered: {
                    playNext()
                }
            }

            MenuItem {
                text: "Add to Queue"
                onTriggered: {
                    addQueue()
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onPressed: root.pressed = true
        onReleased: {
            root.pressed = false

            if(mouse.button === Qt.LeftButton) {
                root.clicked()
                MediaPlayer.play(all_songs.toSongModel(root), ind)
            }else if(mouse.button === Qt.RightButton) {
                contextMenu.popup()
            }
        }
    }

}



