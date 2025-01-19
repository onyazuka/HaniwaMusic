import QtQuick 2.15
import QtQuick.Controls

RoundButton {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    id: button
    width: height
    Text {
        objectName: "text"
        anchors.centerIn: parent
        color: palette.highlight
        // text is set by qt
    }
}
