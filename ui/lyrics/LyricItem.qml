//LyricItem.qml
import QtQuick
import QtQuick.Controls
import Nebula.Media
import Nebula.Events

Rectangle {
    id: root
    property int time
    property string text: ""
    property bool active: false
    property int idx
    // keep a fixed height based on full-size text so scaling won't reflow delegate
    height: label.implicitHeight * 1.25

    color: "transparent"

    Label {
        id: label
        text: root.text
        font.pointSize: 16         // always layout at 16sp
        font.bold: true
        wrapMode: Label.WordWrap
        verticalAlignment: Text.AlignTop
        horizontalAlignment: Text.AlignLeft
        anchors.fill: parent
        width: parent.width

        // cache only when downscaled, let full-res redraw when active
        layer.enabled: !root.active
        layer.smooth: true
        transformOrigin: Item.TopLeft

        // scale down to 14sp when inactive
        property real inactiveScale: 14.0/16.0
        scale: root.active ? 1.0 : inactiveScale

        // animate scale + color
        Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
        Behavior on color { ColorAnimation  { duration: 200; easing.type: Easing.InOutQuad } }

        states: [
            State {
                name: "inactive"
                when: !root.active
                PropertyChanges { target: label; color: Qt.rgba(1,1,1,0.5) }
            },
            State {
                name: "active"
                when: root.active
                PropertyChanges { target: label; color: "white" }
            }
        ]
    }

    Connections {
        target: MediaPlayer

        // function onTime_changed() {
        //     active = EventBus.getCurrentLyricIndex() === idx
        //     if (MediaPlayer.get_position() >= root.time) {
        //         if (EventBus.getCurrentLyricIndex() !== idx) {
        //             EventBus.setCurrentLyricIndex(idx)
        //             active = true
        //             EventBus.emitLyricsViewChanged()
        //         }
        //     }
        // }

        function onTime_changed() {
            var inframe = MediaPlayer.isInFrame(time, idx)
            if (inframe) {
                active = true
                EventBus.setCurrentLyricIndex(idx)
                EventBus.emitLyricsViewChanged()
            }else {
                active = false
            }
        }
    }
}
