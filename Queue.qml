import QtQuick
import QtQuick.Controls.Material
import Nebula.QueueModel
import "Consts.js" as Consts
import QtQuick.Layouts

Rectangle {

    id: root
    color: Consts.player_color

    Seperator {
        id: br
        h: parent.height
        anchors.left: root.left
        clr: Qt.lighter(Consts.player_color)
        z: 5
    }

    Label {
        id: playing_next_label
        text: "Playing Next: "
        font {
            pointSize: 18
            bold: true
        }
        color: "white"

        anchors {
            top: parent.top
            left: br.right
            margins: 20
        }
    }

    Seperator {
        id: label_br
        w: root.width
        clr: Qt.lighter(Qt.lighter(Consts.player_color))
        anchors.top: playing_next_label.bottom
        anchors.left: root.left
        anchors.topMargin: 20
    }

    // Queue_item {
    //     title: "Test"
    //     anchors {
    //         top: label_br.bottom
    //         horizontalCenter: parent.horizontalCenter
    //         topMargin: 20
    //     }
    // }

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

        model: Model

        delegate: Queue_item {
            id: cell
            title: modelData.title
            width: listview.width * 0.95

            Component.onCompleted: {
                try {
                    anchors.horizontalCenter = parent.horizontalCenter
                } catch (error){

                }
            }

            anchors.horizontalCenter: parent.horizontalCenter

            required property var modelData
        }
    }
}
