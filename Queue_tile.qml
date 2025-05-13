import QtQuick

Rectangle {
    id: root
    required property Item dragParent
    signal pressed
    signal released
    signal clicked

    property int visualIndex: 0

    anchors {
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }
    radius: 3

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: root
        onClicked: root.clicked()
        onPressed: root.pressed()
        onReleased: {
            parent.Drag.drop()
            root.released()
        }
    }

    Drag.active: mouseArea.drag.active
    Drag.source: root
    Drag.hotSpot.x: root.width / 2
    Drag.hotSpot.y: root.height / 2

    states: [
        State {
            when: mouseArea.drag.active
            ParentChange {
                target: root
                parent: root.dragParent
            }

            AnchorChanges {
                target: root
                // anchors.horizontalCenter: undefined
                anchors.verticalCenter: undefined
            }
        }
    ]
}
