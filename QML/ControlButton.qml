import QtQuick 2.15
import QtQuick.Controls

RoundButton {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    property string txt;
    id: button
    width: height
    Text {
        id: text
        anchors.centerIn: parent
        color: palette.highlight
        text: button.txt
    }
}
