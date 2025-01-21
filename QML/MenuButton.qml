import QtQuick 2.15
import QtQuick.Controls

Button {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    id: button
    height: implicitHeight * 1.25
    width: height
    icon.color: pal.highlight
    /*Text {
        objectName: "text"
        anchors.centerIn: parent
        color: palette.highlight
        // text is set by qt
    }*/
}
