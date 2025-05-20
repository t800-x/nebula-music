import QtQuick
import QtQuick.Shapes
import QtQuick.Effects

Rectangle {
    id: root
    color: "transparent"
    property var blurSource
    property real alpha: 0.02
    clip: true

    ShaderEffectSource {
        id: effectSource
        sourceItem: blurSource
        scale: 0.1
        sourceRect: Qt.rect(root.x, root.y, root.width, root.height)
        height: blurSource.height / 4
        width: blurSource.width / 4
        smooth: true
    }

    MultiEffect {
        id: blurEffect
        anchors.fill: parent
        source: effectSource
        blurEnabled: true
        blurMax: 64
        blur: 1.0
    }
}
