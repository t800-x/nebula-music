import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

Rectangle {
    id: root
    property alias text: label.text
    // property alias iconSource: icon.source
    property alias visibility: root.visible
    signal clicked
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width * 0.9
    height: label.font.pixelSize * 1.85
    radius: 8

    property bool hovered: false
    property bool pressed: false
    property color baseColor: Consts.nav_color

    visible: true
    color: {
        var changed_color = adjustBrightness(baseColor, 1.2)

        if (visible && hovered) {
            return adjustBrightness(changed_color, 1.2)
        }else if (visible || hovered){
            return changed_color
        }else{
            return baseColor
        }
    }

    //(hovered || visible) ? adjustBrightness(baseColor, 1.2) : baseColor

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onPressed: root.pressed = true
        onReleased: {
            root.pressed = false
            root.clicked()   //Expose this signal
        }
    }

    Row {
        anchors.top: parent.top
        anchors.left: parent.left
        leftPadding: 12
        spacing: 10

        Label {
            id: icon
            text: ""
            font.family: "CupertinoIcons"
            color: "white"
            font.pointSize: 14
            y: +3
        }

        // Image {
        //     id: icon
        //     source: ""
        //     width: 20
        //     height: 20
        //     y: +2
        // }

        Label {
            id: label
            font.pixelSize: 14
            color: "white"
            y: +3
        }
    }

    function adjustBrightness(color, factor) {
        let r = color.r * factor;
        let g = color.g * factor;
        let b = color.b * factor;

        r = Math.min(1.0, r);
        g = Math.min(1.0, g);
        b = Math.min(1.0, b);

        return Qt.rgba(r, g, b, color.a);
    }
}
