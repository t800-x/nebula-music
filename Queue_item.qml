import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

Rectangle {
    property int index: 1
    property int fontsize: 12
    required property string title
    property bool hovered: false
    height: song_title.font.pointSize * 2
    radius: 8
    color: Consts.main_bg_color
    visible: true
    clip: true

    Label {
        id: song_title
        text: title
        color: "white"
        font.pointSize: fontsize
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 5
    }

    // MouseArea {
    //     anchors.fill: parent
    //     hoverEnabled: true
    //     onEntered: hovered = true
    //     onExited: hovered = false
    // }
}
