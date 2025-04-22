import QtQuick
import QtQuick.Controls.Material
import "Consts.js" as Consts

Rectangle {
    width: row.implicitWidth
    height: parent.height
    color: "transparent"
    signal prev_song
    signal plause_song
    signal next_song
    id: root
    // border.color: "red" <-- Debug

    Row {
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        id: row

        Rectangle {
            id: prev
            width: prev_btn.implicitWidth
            height: prev_btn.implicitHeight
            color: "transparent"
            Image {
                id: prev_btn
                source: "icons/prev.svg"
                smooth: false
                asynchronous: true // optional, avoids blocking
                cache: true        // optional, avoids reloads
                fillMode: Image.PreserveAspectFit
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
            Image {
                id: plause_btn
                source: "icons/pause.svg"
                smooth: false
                asynchronous: true // optional, avoids blocking
                cache: true        // optional, avoids reloads
                fillMode: Image.PreserveAspectFit
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
            Image {
                id: next_btn
                source: "icons/next.svg"
                smooth: false
                asynchronous: true // optional, avoids blocking
                cache: true        // optional, avoids reloads
                fillMode: Image.PreserveAspectFit
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
