#include "QMLSlider.hpp"

QMLSlider::QMLSlider(Qt::Orientation orientation, QWidget* parent)
    : QMLWidgetBase(":/QML/Slider.qml", parent)
{
    setOrientation(orientation);
}

void QMLSlider::setOrientation(Qt::Orientation orientation) {
    if (orientation == Qt::Orientation::Horizontal) {
        slider()->setProperty("orientation", 1);
    }
    else {
        slider()->setProperty("orientation", 2);
    }
}

QQuickItem* QMLSlider::slider() {
    return findItem("slidan");
}
