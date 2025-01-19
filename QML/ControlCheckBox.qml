import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    spacing: 0
    Switch {
        id: chBox
        objectName: "chBox"
        padding: 0
        property string iconPath
        property bool checkState
        //text: txt
        position: checkState ? 1.0 : 0.0
    }
    Button {
        implicitHeight: chBox.height
        implicitWidth: chBox.height
        icon.source: chBox.iconPath
        icon.width: implicitWidth * 0.8
        icon.height: implicitHeight * 0.8
        icon.color: pal.text
        display: AbstractButton.IconOnly
        flat: true
        padding: 0
        background: Rectangle {
            color: "white"
        }
    }
}

