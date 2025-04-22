import QtQuick
import "Consts.js" as Consts

Rectangle {
    property real h: 1.5
    property real w: 1.5
    required property string clr

    color: clr
    height: h
    width: w
    visible: true
}
