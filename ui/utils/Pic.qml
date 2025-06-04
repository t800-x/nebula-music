import QtQuick
import QtQuick.Effects

Item {
    id: root
    property int radius: 5
    property string source: "qrc:/qt/qml/nebula-music/icons/placeholder.png"
    property bool visibility: true

    Image {
        id: cover
        source: root.source
        visible: false
        mipmap: true
        asynchronous: true
        cache: true

        anchors.fill: parent
    }

    // MultiEffect mask for rounded corners
    MultiEffect {
        id: cover_render

        anchors.fill: cover
        source: cover

        maskEnabled: true                           // Turn on masking :contentReference[oaicite:4]{index=4}
        maskSource: roundedMask                      // Use our rectangle as mask :contentReference[oaicite:5]{index=5}

        // Anti-aliasing tweaks (optional but recommended)
        maskThresholdMin: 0.5                        // Sharpness threshold :contentReference[oaicite:6]{index=6}
        maskSpreadAtMin: 1.0

        visible: root.visibility
    }

    // Mask definition
    Item {
        id: roundedMask
        width: cover.width
        height: cover.height
        visible: false                               // Hide mask itself

        layer.enabled: true                          // Required for maskSource :contentReference[oaicite:8]{index=8}
        layer.smooth: true                           // Smooth out edges :contentReference[oaicite:9]{index=9}

        Rectangle {
            width: parent.width
            height: parent.height
            radius: root.radius                               // Desired corner radius :contentReference[oaicite:10]{index=10}
            color: "#ff000000"                       // Only alpha channel is used for masking
        }
    }
}
