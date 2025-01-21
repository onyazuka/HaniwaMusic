#include "QMLIcon.hpp"

QMLIcon::QMLIcon(const QString& iconSource, QWidget* parent)
    : QMLWidgetBase(":/QML/Icon.qml", parent)
{
    icon_source = QQmlProperty(base(), "icon.source");
    icon_source.write(iconSource);
}

