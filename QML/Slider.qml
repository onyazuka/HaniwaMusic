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
