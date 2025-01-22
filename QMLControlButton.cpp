#include "QMLControlButton.hpp"

QMLControlButton::QMLControlButton(const QString& iconSource, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlButton.qml", parent)
{
    icon_source = QQmlProperty(base(), "icon.source");
    icon_source.write(iconSource);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QQuickItem* QMLControlButton::base() {
    return findItem("button");
}
