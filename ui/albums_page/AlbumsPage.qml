import QtQuick
import Nebula.Database

Rectangle {
    id: root
    color: "transparent"

    property var albums: Keeper.ready ? Keeper.get_albums_data() : {}
    anchors.fill: parent
    AlbumHeader {
        id: header
        table: root.albums
        z: 1
    }

    GridView {
        id: grid
        z: 0
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 25
        }

        model: root.albums

        cellHeight: 225
        cellWidth: 200

        delegate: AlbumItem {
            title: modelData.title
            artist: modelData.artist
            albumId: modelData.albumId

            height: grid.cellHeight
            width: grid.cellWidth

            required property var modelData
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
        }
    }
}
