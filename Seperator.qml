import QtQuick
import "Consts.js" as Consts

Rectangle {
    property real h: 1
    property real w: 1
    required property string clr

    color: clr
    height: h
    width: w
    visible: true
}
