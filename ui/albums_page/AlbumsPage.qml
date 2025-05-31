import QtQuick

Rectangle {
    anchors.fill: parent
    color: "transparent"

    AlbumHeader {}

    AlbumItem {
        anchors.centerIn: parent
        width: 150
        height: 150
    }
}
