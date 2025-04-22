import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media

Rectangle {
    height: parent.height
    width: parent.width
    color: "transparent"
    z: 1

    Song_item {
        id: item
        song_name: "Timeless"
        img_source: "test_assets/cover.jpg"
        artist: "The Weeknd"
        album: "Hurry Up Tomorrow"
        path: "F:/Music/Timeless.m4a"


        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            leftMargin: 10
            topMargin: 30 + Consts.player_height
            rightMargin: 10
        }
    }
}
