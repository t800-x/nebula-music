import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

Rectangle {
    id: root
    height: 18 * 1.85
    color: Consts.player_color

    Grid {
        id: songGrid
        anchors.fill: parent
        anchors.leftMargin: 13
        anchors.rightMargin: 15
        anchors.topMargin: 4
        anchors.bottomMargin: 4
        rows: 1
        columns: 13
        flow: Grid.LeftToRight

        // calculate half of spacing minus separator width (20px spacing, 1px separator)
        property real spacerWidth: (20 - 1) / 2

        // Separator before Title
        Seperator {
            h: root.height
            clr: Qt.lighter(Consts.player_color)
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Column 1: Song Name (33% width)
        Label {
            width: root.width * 0.33
            text: "Title"
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 18
            font.bold: true
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Separator between Title and Artist
        Seperator {
            h: root.height
            clr: Qt.lighter(Consts.player_color)
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Column 2: Artist (33% width)
        Label {
            width: root.width * 0.33
            text: "Artist"
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 18
            font.bold: true
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Separator between Artist and Album
        Seperator {
            h: root.height
            clr: Qt.lighter(Consts.player_color)
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Column 3: Album (33% width)
        Label {
            width: root.width * 0.33
            text: "Album"
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 18
            font.bold: true
        }
        // Spacer
        Item { width: songGrid.spacerWidth; height: root.height }

        // Separator after Album
        Seperator {
            h: root.height
            clr: Qt.lighter(Consts.player_color)
        }
    }
}
