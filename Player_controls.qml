import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts
import Nebula.Media

Rectangle {
    width: row.implicitWidth
    height: parent.height
    color: "transparent"
    property bool paused: false
    signal prev_song
    signal plause_song
    signal next_song
    id: root
    // border.color: "red" <-- Debug

    Row {
        spacing: 20
        anchors.verticalCenter: parent.verticalCenter
        id: row

        Rectangle {
            id: prev
            width: prev_btn.implicitWidth
            height: prev_btn.implicitHeight
            color: "transparent"
            Label {
                id: prev_btn
                text: ""
                font.family: "CupertinoIcons"
                color: "white"
                font.pointSize: 20
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.prev_song()
                }
            }
        }

        Rectangle {
            id: plause
            width: plause_btn.implicitWidth
            height: plause_btn.implicitHeight
            color: "transparent"
            Label {
                id: plause_btn
                text: {
                    if (paused) {
                        return ""
                    } else {
                        return ""
                    }
                }

                font.family: "CupertinoIcons"
                color: "white"
                font.pointSize: 25
                y: -3
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.plause_song()
                }
            }
        }

        Rectangle {
            id: next
            width: next_btn.implicitWidth
            height: next_btn.implicitHeight
            color: "transparent"
            Label {
                id: next_btn
                text: ""
                font.family: "CupertinoIcons"
                color: "white"
                font.pointSize: 20
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.next_song()
                }
            }
        }
    }
}
