import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Nebula.Events
import Nebula.Database
import Nebula.Media

import "../utils"
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    color: "transparent"
    id: root
    property int albumId
    property string cover_img: Keeper.getAlbumCover(albumId)
    property string title
    property string artist
    property var songs: Keeper.get_album_songs(albumId, root)
    property int idx

    AlbumHeader {
        id: header
        text: ""

        BackButton {
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                margins: 8
            }

            onClicked: EventBus.emitPopMainCanvas()
        }

        IconButton {
            xHeight: 1.5
            xWidth: 2.8

            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 8
            }

            text: "\uF4A5"

            onClicked: active = !active
        }

        TextBox {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: 25
            }
            height: 32
            width: 200

            onChanged: root.songs.setFilterText(txt)
        }
    }


    Pic {
        id: cover
        source: root.cover_img
        anchors {
            left: parent.left
            top: header.bottom
            margins: 35
        }

        height: 250
        width: 250
    }

    Column {
        id: albumInfo
        spacing: 2.5

        x: cover.width + cover.anchors.margins + 25 // <--Margin
        y: cover.height/1.5

        Label {
            text: root.title
            color: "white"
            font.pointSize: 20
            font.bold: true
        }

        Label {
            text: root.artist
            color: Consts.accent
            font.pointSize: 16
        }
    }

    Row {
        anchors {
            bottom: cover.bottom
            bottomMargin: 5
        }

        x: cover.width + cover.anchors.margins + 25
        spacing: 10

        PlaybackButton {
            text: "Play"
            icon: "\uF488"
        }

        PlaybackButton {
            text: "Shuffle"
            icon: "\uF4A8"
        }
    }

    ListView {
        id: listview
        anchors.top: cover.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 25

        spacing: 5

        model: root.songs

        delegate: ListDelegate {
            title: modelData.title
            num: index + 1
            idx: index
            path: modelData.path
            width: parent?.width
            anchors.horizontalCenter: parent?.horizontalCenter
            table: root.songs


            required property var modelData
            required property int index
        }
    }


}
