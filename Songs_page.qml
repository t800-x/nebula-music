import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media
import Nebula.Database

Rectangle {
    height: parent.height
    width: parent.width
    color: "transparent"
    property var table: {
        var x = Keeper.get_table();
        console.log(JSON.stringify(x))
        return x
    }

    z: 1

    ListView {
        id: listview
        width: parent.width
        height: parent.height
        // spacing: 10

        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            // leftMargin: 10
            topMargin: 30 + Consts.player_height
            // rightMargin: 10
        }

        model: table

        delegate: Song_item {
            song_name: modelData.title
            img_source: ""
            artist: modelData.artist
            album: modelData.album
            path: modelData.path
            alt: index % 2 === 0 ? true : false

            required property var modelData
            required property int index
        }
    }

//     Song_item {
//         id: item
//         song_name: "Timeless"
        // img_source: "test_assets/cover.jpg"
        // artist: "The Weeknd"
        // album: "Hurry Up Tomorrow"
        // path: "F:/Music/Timeless.m4a"


        // anchors{
        //     top: parent.top
        //     left: parent.left
        //     right: parent.right
        //     leftMargin: 10
        //     topMargin: 30 + Consts.player_height
        //     rightMargin: 10
        // }
//     }
}
