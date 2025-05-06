import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media
import QtQuick.Effects

Rectangle {
    required property string song_name
    required property string img_source
    property int ind
    property bool alt
    property string artist: "Unknown"
    property string album: "Unknown"
    required property string path
    signal clicked

    property bool hovered: false
    property bool pressed: false

    id: root

    // width: parent.width
    height: 14 * 1.85
    color: {
        var changed_color = Qt.lighter(Consts.main_bg_color)
        if (hovered) {
            return Qt.lighter(changed_color)
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
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onPressed: root.pressed = true
        onReleased: {
            root.pressed = false
            root.clicked()
            MediaPlayer.play(ind)
        }
    }
}



