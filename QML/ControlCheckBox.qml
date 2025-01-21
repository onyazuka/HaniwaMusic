import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Control {
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    spacing: 0
    background: Rectangle {
        implicitWidth: chBox.implicitWidth + icon.implicitWidth
        implicitHeight: chBox.implicitHeight
        color: pal.window
    }

    Switch {
        id: chBox
        objectName: "chBox"
        padding: 0
        property string iconPath
        // property checked set by cpp
    }
    Icon {
        x: chBox.implicitWidth
        id: icon
        implicitHeight: chBox.height
        implicitWidth: chBox.height
        icon.source: chBox.iconPath
    }
}

