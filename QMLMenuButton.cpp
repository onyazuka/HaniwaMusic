#include "QMLMenuButton.hpp"

QMLMenuButton::QMLMenuButton(const QString& iconSource, QWidget* parent)
    : QMLWidgetBase(":/QML/MenuButton.qml", parent)
{
    icon_source = QQmlProperty(base(), "icon.source");
    icon_source.write(iconSource);
}
