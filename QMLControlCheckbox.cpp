#include "QMLControlCheckbox.hpp"

QMLControlCheckbox::QMLControlCheckbox(const QString& path, bool chState, QWidget* parent)
    : QMLWidgetBase(":/QML/ControlCheckBox.qml", parent)
{
    setIconPath(path);
    setChecked(chState);
}

void QMLControlCheckbox::setIconPath(const QString& path) {
    checkBox()->setProperty("iconPath", path);
}

void QMLControlCheckbox::setChecked(bool chState) {
    checkBox()->setProperty("checked", chState);
}

bool QMLControlCheckbox::isChecked() const {
    return checkBox()->property("checked").toBool();
}

QQuickItem* QMLControlCheckbox::checkBox() const {
    return findItem("chBox");
}
