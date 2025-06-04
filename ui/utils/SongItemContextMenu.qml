import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Menu {
    id: root
    signal playNext()
    signal addQueue()
    MenuItem {
        text: "Play Next"
        onTriggered: {
            root.playNext()
        }
    }

    MenuItem {
        text: "Add to Queue"
        onTriggered: {
            root.addQueue()
        }
    }
}
