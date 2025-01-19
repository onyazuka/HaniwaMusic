#include "QMLControlButton.hpp"

QMLControlButton::QMLControlButton(const QString& text, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlButton.qml", parent)
{
    setText(text);
}

void QMLControlButton::setText(const QString& text) {
    findItem("text")->setProperty("text", text);
}
