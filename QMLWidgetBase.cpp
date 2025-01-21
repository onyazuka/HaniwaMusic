#include "QMLWidgetBase.hpp"

QMLWidgetBase::QMLWidgetBase(const QString& resourcePath, QWidget* parent)
    : QQuickWidget(parent)
{
    setSource(QUrl::fromLocalFile(resourcePath));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
}

QMLWidgetBase::~QMLWidgetBase() {
    ;
}

QQuickItem* QMLWidgetBase::findItem(const QString& objectName) {
    QQuickItem* it = rootObject()->findChild<QQuickItem*>(objectName);
    return it;
}

QQuickItem* QMLWidgetBase::base() {
    return rootObject();
}
