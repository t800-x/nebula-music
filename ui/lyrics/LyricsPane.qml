// LyricsPane.qml
import QtQuick 2.15
import QtQuick.Controls 6.3
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import "../utils"
import Nebula.Media
import Nebula.Events

Rectangle {
    id: root
    property string name: "LyricsPane"
    property var lyrics: MediaPlayer.get_synced_lyrics()
    color: Consts.player_color

    Behavior on width {
        NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
    }

    Seperator {
        h: parent.height
        anchors.left: parent.left
    }

    ListView {
        id: view
        anchors.fill: parent
        model: lyrics
        interactive: false
        highlightMoveDuration: 500
        highlightRangeMode: ListView.ApplyRange
        anchors.margins: 7
        anchors.bottomMargin: 60
        cacheBuffer: contentHeight

        property int currentLine: -1

        highlight: Rectangle {
            color: "transparent"
            border.color: "transparent"
            width: 0
            height: 0
        }

        preferredHighlightBegin: 40
        preferredHighlightEnd: 40

        onCurrentLineChanged: positionViewAtIndex(view.currentIndex, ListView.Beginning)

        delegate: LyricItem {
            time: modelData.timestamp_ms
            text: modelData.text
            width: parent.width
            anchors.left: parent?.left
            anchors.right: parent?.right
            idx: index

            required property var modelData
            required property int index
        }
    }

    Connections {
        target: EventBus

        function onLyricsViewChanged() {
            view.currentIndex = EventBus.getCurrentLyricIndex()
        }
    }

    Connections {
        target: MediaPlayer

        function onSongChanged() {
            EventBus.setCurrentLyricIndex(-1);
            view.currentIndex = -1
            view.positionViewAtIndex(0, ListView.Beginning)
        }
    }
}
