import QtQuick 6.8
import QtQuick.Controls
import QtQuick.Layouts


Control {
    id: root
    SystemPalette { id: pal; colorGroup: SystemPalette.Active }
    background: Rectangle {
        anchors.fill: parent
        color: pal.window
    }
    implicitHeight: control.implicitHeight
    implicitWidth: control.implicitWidth
    Slider {
        id: control
        objectName: "slider"
        value: 0.0
        background: Rectangle {
            x: control.horizontal ? control.leftPadding : control.leftPadding + control.availableWidth / 2 - width / 2
            y: control.horizontal ? control.topPadding + control.availableHeight / 2 - height / 2 : control.topPadding
            implicitWidth: control.horizontal ? root.width - control.leftPadding : 10
            implicitHeight: control.horizontal ? 10 : root.height - control.topPadding
            width: implicitWidth
            height: implicitHeight
            radius: 10
            color: pal.window
            border.color: drawingCanvas.color2
            z: 2

            Canvas
            {
                property color color2: "#ccc"
                id: drawingCanvas
                anchors.fill: parent
                onPaint:
                {
                    var ctx = getContext("2d")
                    ctx.lineWidth = 1;
                    ctx.strokeStyle = drawingCanvas.color2
                    if (control.horizontal) {
                        for (let i = 50; i < drawingCanvas.width; i += 50) {
                            ctx.beginPath()
                            ctx.moveTo(i, 0)
                            ctx.lineTo(i, drawingCanvas.height)
                            ctx.closePath()
                            ctx.stroke()
                        }
                    }
                    else {
                        for (let i = 50; i < drawingCanvas.height; i += 50) {
                            ctx.beginPath()
                            ctx.moveTo(0, i)
                            ctx.lineTo(drawingCanvas.width, i)
                            ctx.closePath()
                            ctx.stroke()
                        }
                    }
                }
            }

            /*gradient: Gradient {
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
            }*/

            Rectangle {
                width: control.horizontal ? control.visualPosition * parent.width : parent.width
                height: control.horizontal ? parent.height : parent.height - y
                y: control.horizontal ? 0 :  control.visualPosition * parent.height
                color: pal.highlight
                radius: 10
            }
        }

        handle: Rectangle {
            id: handle
            implicitWidth: 26
            implicitHeight: 26
            z: 3
            x: control.horizontal ? control.leftPadding + control.visualPosition * (control.width - width) :  control.leftPadding + control.availableWidth / 2 - width / 2
            y: control.horizontal ? control.topPadding + control.availableHeight / 2 - height / 2 : control.topPadding + control.visualPosition * (control.height - height)
            radius: 13
            color: control.pressed ? "#f0f0f0" : "#f6f6f6"
            border.color: "#bdbebf"
        }
    }
}
