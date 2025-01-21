import QtQuick 6.8
import QtQuick.Controls

Button {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    //anchors.centerIn: parent
    //implicitHeight: 20
    //implicitWidth: 20
    //icon.source: "qrc:/icons/random.svg"
    //icon.width: implicitWidth * 0.8
    //icon.height: implicitHeight * 0.8
    icon.color: pal.text
    display: AbstractButton.IconOnly
    flat: true
    padding: 0
    background: Rectangle {
        color: pal.window
    }
}
