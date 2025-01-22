import QtQuick 6.8
import QtQuick.Controls
import QtQuick.Layouts

Control {
    background: Rectangle {
        implicitWidth: button.implicitWidth
        implicitHeight: button.implicitHeight
        color: pal.window
    }

    RoundButton {
        SystemPalette { id: pal; colorGroup: SystemPalette.Active }
        id: button
        objectName: "button"
        width: height
        icon.color: pal.highlight
        /*Text {
            id: text
            objectName: "text"
            anchors.centerIn: parent
            color: palette.highlight
            // text is set by qt
        }*/
    }
}
