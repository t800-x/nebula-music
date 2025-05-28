import QtQuick
import QtQuick.Controls
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts

Rectangle {
    id: button
    property string text: ""
    property bool pressed: false
    property bool hovered: false
    property bool active: false
    signal clicked()

    radius: 6
    width: btn_text.implicitWidth * 1.5
    height: btn_text.implicitHeight * 1.25
    anchors.verticalCenter: parent.verticalCenter
    clip: true

    // Define the three visual states
    states: [
        State {
            name: "inactive"
            when: !button.hovered && !button.active
            PropertyChanges { target: button; color: "transparent" }
        },
        State {
            name: "hovered"
            when: button.hovered && !button.active
            PropertyChanges { target: button; color: Qt.rgba(1, 1, 1, 0.1) }
        },
        State {
            name: "active"
            when: button.active
            PropertyChanges { target: button; color: Consts.accent }
        }
    ]

    // Animate only the transition from inactive → active
    transitions: [
        Transition {
            from: "inactive"; to: "active"
            ColorAnimation {
                property: "color"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        },

        Transition {
            from: "hovered"; to: "active"
            ColorAnimation {
                property: "color"
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    ]

    Label {
        id: btn_text
        text: button.text
        font.family: "CupertinoIcons"
        color: button.pressed ? "#787878" : "white"
        font.pointSize: 18
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: button.clicked()
        onEntered: button.hovered = true
        onExited: button.hovered = false
        onPressed: button.pressed = true
        onReleased: button.pressed = false
    }
}
