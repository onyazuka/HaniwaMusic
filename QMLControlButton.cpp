#include "QMLControlButton.hpp"

QMLControlButton::QMLControlButton(const QString& txt, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlButton.qml", parent)
{
    text = QQmlProperty(findItem("text"), "text");
    text.write(txt);
}
