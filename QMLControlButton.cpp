#include "QMLControlButton.hpp"

QMLControlButton::QMLControlButton(const QString& txt, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlButton.qml", parent)
{
    text = QQmlProperty(findItem("text"), "text");
    text.write(txt);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QQuickItem* QMLControlButton::base() {
    return findItem("button");
}
