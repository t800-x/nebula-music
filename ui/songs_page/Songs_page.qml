import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import Nebula.Media
import Nebula.Database

Rectangle {
    anchors.fill: parent
    color: "transparent"
    clip: true
    id: root
    property var table: Keeper.get_all_songs()


    Songs_title {
        id: title
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
        }

        width: parent.width
    }

    Seperator {
        id: title_br
        height: 1
        clr: Qt.lighter(Consts.player_color)
        anchors.top: title.bottom
        z: 2
    }

    Songs_header {
        id: header
        anchors {
            top: title_br.bottom
            left: parent.left
            right: parent.right
        }
        z: 2
    }

    Seperator {
        id: header_br
        height: 1
        clr: Qt.lighter(Consts.player_color)
        anchors.top: header.bottom
        z: 2
    }

    DelegateModel {
        id: dgModel
        model: table
        delegate: Song_item {
            song_name: modelData.title
            img_source: modelData.cover
            artist: modelData.artist
            album: modelData.album
            path: modelData.path
            alt: index % 2 === 0 ? false : true
            width: listview.width
            ind: index
            all_songs: table

            onAddQueue: {
                MediaPlayer.add_to_queue(modelData)
            }

            onPlayNext: {
                MediaPlayer.play_next(modelData)
            }

            required property var modelData
            required property int index
        }
    }

    ListView {
        id: listview
        spacing: 4
        z: 0
        ScrollBar.vertical: ScrollBar {}
        anchors {
            top: header_br.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            // bottomMargin: 75
            leftMargin: 4
            rightMargin: 4
            topMargin: 4
        }

        model: dgModel
        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
        }
    }
}

