#include "QMLControlCheckbox.hpp"

QMLControlCheckbox::QMLControlCheckbox(const QString& path, bool chState, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlCheckBox.qml", parent)
{
    checked = QQmlProperty(base(), "checked");
    iconPath = QQmlProperty(base(), "iconPath");
    checked.write(chState);
    iconPath.write(path);
}

QQuickItem* QMLControlCheckbox::base() {
    return findItem("chBox");
}
