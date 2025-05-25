import QtQuick
import QtQuick.Controls.Material
import "qrc:/qt/qml/nebula-music/Consts.js" as Consts
import QtQuick.Layouts
import QtQml.Models
import Nebula.Media
import "../utils/Seperator.qml"

Rectangle {

    id: root
    property var song_model: MediaPlayer.get_queue()
    color: Consts.player_color

    Seperator {
        id: br
        h: parent.height
        anchors.left: root.left
        z: 5
    }

    Label {
        id: playing_next_label
        text: "Up Next"
        font {
            pointSize: 10
            bold: true
        }
        color: "white"

        anchors {
            top: parent.top
            left: br.right
            margins: 7
        }
    }

    CustomButton {
        text: "Clear"

        anchors {
            right: parent.right
            top: parent.top
            rightMargin: 7
            topMargin: 5
        }

        onClicked: console.log("Clear")
    }

    Seperator {
        id: label_br
        w: root.width
        // clr: Qt.lighter(Qt.lighter(Consts.player_color))
        anchors.top: playing_next_label.bottom
        anchors.left: root.left
        anchors.topMargin: 7
    }

    ListView {
        id: listview
        width: parent.width
        height: parent.height
        z: 5
        spacing: 10

        anchors {
            top: label_br.bottom
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.OutQuad
            }
        }

        /*
            I don't yet fully understand how it works.
            https://raymii.org/s/tutorials/Qml_Drag_and_Drop_example_including_reordering_the_Cpp_Model.html
        */

        model: DelegateModel {
            id: visualModel
            model: song_model
            delegate: DropArea {
                id: delegateRoot
                required property var modelData
                property int visualIndex: DelegateModel.itemsIndex
                property int modelIndex

                height: queue_item.fontsize * 2
                width: listview.width * 0.95
                anchors.horizontalCenter: parent?.horizontalCenter

                onEntered: function (drag) {
                    var from = (drag.source as Queue_tile).visualIndex
                    var to = tile.visualIndex
                    visualModel.items.move(from, to)
                }

                onDropped: function (drag) {
                    var from = modelIndex
                    var to = (drag.source as Queue_tile).visualIndex
                    MediaPlayer.move(from, to)
                }

                Queue_tile {
                    id: tile
                    height: queue_item.fontsize * 2
                    width: listview.width * 0.95
                    dragParent: listview
                    visualIndex: delegateRoot.visualIndex
                    onPressed: delegateRoot.modelIndex = visualIndex
                    color: "transparent"
                    z: 20

                    Queue_item {
                        id: queue_item
                        title: delegateRoot.modelData.title
                        anchors.centerIn: parent
                        width: listview.width * 0.95
                    }
                }
            }
        }
    }
}
