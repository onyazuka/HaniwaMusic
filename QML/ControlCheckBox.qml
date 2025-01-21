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
        // property checked set by cpp
    }
    Icon {
        implicitHeight: chBox.height
        implicitWidth: chBox.height
        icon.source: chBox.iconPath
    }
}

