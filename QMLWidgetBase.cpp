#include "QMLWidgetBase.hpp"

QMLWidgetBase::QMLWidgetBase(const QString& resourcePath, QWidget* parent)
    : QQuickWidget(parent)
{
    setSource(QUrl::fromLocalFile(resourcePath));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QQuickItem* QMLWidgetBase::findItem(const QString& objectName) const {
    return rootObject()->findChild<QQuickItem*>(objectName);
}
