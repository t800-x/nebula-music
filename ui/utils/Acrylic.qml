//Context.qml

import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root
    color: "transparent"
    property var blurSource
    property real alpha : 0.02
    clip: true

    Rectangle {
        id: maskShape
        anchors.fill: parent
        radius: 8             // <-- your desired corner radius
        color: "black"        // mask uses the alpha channel of this
        visible: false        // hide the shape itself
    }

    Item {
        id: blurredContent
        anchors.fill: parent
        clip: true

        ShaderEffectSource {
            id: effectSource
            sourceItem: blurSource
            anchors.fill: parent
            live: true
            scale: 0.1
            sourceRect: Qt.rect(root.x, root.y, root.width, root.height)
        }

        FastBlur {
            id: blur
            anchors.fill: effectSource
            source: effectSource
            radius: 128
        }

        FastBlur {
            id: blur2
            anchors.fill: blur
            source: blur
            radius: 128
        }

        GaussianBlur {
            id: finalblur
            anchors.fill: blur2
            source: blur2
            radius: 8
            samples: 17
            deviation: 12
        }

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(255, 255, 255, root.alpha)
        }
    }

    // OpacityMask {
    //     anchors.fill: parent
    //     source: blurredContent          // everything above: blur + tint
    //     maskSource: maskShape   // our hidden rounded‐rect
    // }
}
