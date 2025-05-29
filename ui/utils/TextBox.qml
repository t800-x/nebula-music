import QtQuick
import QtQuick.Controls
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts


TextField {
    id: field
    property string txt
    property var pcolor: Qt.rgba(0, 0, 0, 0.3)
    property var scolor: Qt.rgba(0, 0, 0, 0.2)
    verticalAlignment: Text.AlignVCenter
    signal changed()
    property bool blink: false
    color: "white"
    font.pointSize: 10

    onTextChanged: {
        txt = text
        field.changed()
    }

    background: Rectangle {
        color: field.blink ? field.pcolor : field.scolor
        radius: 7
        border.width: blink ? 2 : 0.8
        border.color: blink ? Consts.accent : "#3b3b3b"
    }

    cursorDelegate: Rectangle {
        id: customCursor
        height: parent.height * 0.4
        width: 2
        color: "white"
        y: parent.height * 0.3
        opacity: blink ? 1 : 0

        // Perfectly symmetric blink
        SequentialAnimation on opacity {
            running: field.activeFocus     // only blink when focused
            loops: Animation.Infinite
            NumberAnimation {
                from: 1
                to: 0
                duration: 500               // fade‑out in 0.5s
                easing.type: Easing.Linear
            }
            NumberAnimation {
                from: 0
                to: 1
                duration: 500               // fade‑in in 0.5s
                easing.type: Easing.Linear
            }
        }
    }

    onActiveFocusChanged: {
        if (activeFocus) blink = true
        else blink = false
    }
}

