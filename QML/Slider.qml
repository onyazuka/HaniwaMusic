import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    Slider {
        SystemPalette { id: pal; colorGroup: SystemPalette.Active }
        id: control
        objectName: "slidan"
        value: 0.5
        leftPadding: 10

        Rectangle {
            anchors.fill: parent
            color: pal.window
            z: 1
        }

        background: Rectangle {
            x: control.horizontal ? control.leftPadding : control.leftPadding + control.availableWidth / 2 - width / 2
            y: control.horizontal ? control.topPadding + control.availableHeight / 2 - height / 2 : control.topPadding
            implicitWidth: control.horizontal ? 200 : 10
            implicitHeight: control.horizontal ? 10 : 200
            width: control.horizontal ? control.availableWidth : implicitWidth
            height: control.horizontal ? implicitHeight : control.availableHeight
            radius: 10
            color: pal.window
            border.color: grad.color2
            z: 2

            gradient: Gradient {
                id: grad
                property color color2: "#ccc"
                orientation: control.horizontal ? Gradient.Horizontal : Gradient.Vertical
                GradientStop { position: 0.0; color: pal.window }
                GradientStop { position: 0.095; color: pal.window }
                GradientStop { position: 0.1; color: grad.color2 }
                GradientStop { position: 0.105; color: pal.window }
                GradientStop { position: 0.195; color: pal.window }
                GradientStop { position: 0.2; color: grad.color2 }
                GradientStop { position: 0.205; color: pal.window }
                GradientStop { position: 0.295; color: pal.window }
                GradientStop { position: 0.3; color: grad.color2 }
                GradientStop { position: 0.305; color: pal.window }
                GradientStop { position: 0.395; color: pal.window }
                GradientStop { position: 0.4; color: grad.color2 }
                GradientStop { position: 0.405; color: pal.window }
                GradientStop { position: 0.495; color: pal.window }
                GradientStop { position: 0.5; color: grad.color2 }
                GradientStop { position: 0.505; color: pal.window }
                GradientStop { position: 0.595; color: pal.window }
                GradientStop { position: 0.6; color: grad.color2 }
                GradientStop { position: 0.605; color: pal.window }
                GradientStop { position: 0.695; color: pal.window }
                GradientStop { position: 0.7; color: grad.color2 }
                GradientStop { position: 0.705; color: pal.window }
                GradientStop { position: 0.795; color: pal.window }
                GradientStop { position: 0.8; color: grad.color2 }
                GradientStop { position: 0.805; color: pal.window }
                GradientStop { position: 0.895; color: pal.window }
                GradientStop { position: 0.9; color: grad.color2 }
                GradientStop { position: 0.905; color: pal.window }
                GradientStop { position: 1.0; color: pal.window }
            }

            Rectangle {
                width: control.horizontal ? control.visualPosition * parent.width : parent.width
                height: control.horizontal ? parent.height : control.visualPosition * parent.height
                color: pal.highlight
                radius: 10
            }
        }

        handle: Rectangle {
            implicitWidth: 26
            implicitHeight: 26
            z: 3
            x: control.horizontal ? control.leftPadding + control.visualPosition * (control.availableWidth - width) :  control.leftPadding + control.availableWidth / 2 - width / 2
            y: control.horizontal ? control.topPadding + control.availableHeight / 2 - height / 2 : control.topPadding + control.visualPosition * (control.availableHeight - height)
            radius: 13
            color: control.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }
    }
}
