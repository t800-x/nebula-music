import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media
import Nebula.Database
import QtQuick.Layouts

Rectangle {
    height: parent.height
    width: parent.width
    color: "transparent"
    id: root
    property var table: {
        var x = Keeper.get_table();
        // console.log(JSON.stringify(x))
        return x
    }

    z: 1

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
        width: parent.width
        clr: Qt.lighter(Consts.player_color)
        anchors.top: title.bottom
    }

    Seperator {
        id: header_top
        width: parent.width
        clr: Qt.lighter(Consts.player_color)
        anchors.top: title_br.bottom
        anchors.topMargin: 30
    }

    Songs_header {
        id: header
        anchors {
            top: header_top.bottom
            left: parent.left
            right: parent.right
        }
    }

    Seperator {
        id: header_br
        width: parent.width
        clr: Qt.lighter(Consts.player_color)
        anchors.top: header.bottom
    }

    ListView {
        id: listview
        width: parent.width
        height: parent.height
        // spacing: 10

        anchors{
            top: header_br.bottom
            left: parent.left
            right: parent.right
        }

        model: table

        delegate: Song_item {
            song_name: modelData.title
            img_source: ""
            artist: modelData.artist
            album: modelData.album
            path: modelData.path
            alt: index % 2 === 0 ? false : true
            width: listview.width

            required property var modelData
            required property int index
        }
    }
}

