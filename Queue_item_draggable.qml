import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.QueueModel

Rectangle {
    id: root
    property int index: 1
    property string title
    property int fontsize: 12
    height: root.fontsize * 2
    color: "transparent"

    Component {
        id: dragDelegate
        property int index: 1
        required property string title

        MouseArea {
            id: dragArea

            property bool held: false

            anchors {
                left: parent?.left
                right: parent?.right
            }
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressAndHold: held = true
            onReleased: held = false

            Rectangle {
                id: content

                property bool hovered: false
                height: song_title.font.pointSize * 2
                radius: 8
                color: content.hovered ? Qt.lighter(Consts.main_bg_color) : Consts.main_bg_color
                visible: true
                clip: true

                Label {
                    id: song_title
                    text: dragDelegate.title
                    color: "white"
                    font.pointSize: root.fontsize
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

            DropArea {
                anchors {
                    fill: parent
                    margins: 10
                }

                onEntered: (drag) => {
                               Model.moveSong(
                                   drag.source.DelegateModel.itemsIndex,
                                   dragArea.DelegateModel.itemsIndex)
                           }
            }
        }
    }
}
