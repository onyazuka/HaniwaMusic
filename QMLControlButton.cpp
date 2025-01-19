#include "QMLControlButton.hpp"

QMLControlButton::QMLControlButton(const QString& text, QWidget* parent)
    : QQuickWidget(parent)
{
    setSource(QUrl::fromLocalFile(":/QML/ControlButton.qml"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setText(text);
}

void QMLControlButton::setText(const QString& text) {
    rootObject()->setProperty("txt", text);
}
