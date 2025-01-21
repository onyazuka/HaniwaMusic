#include "QMLSlider.hpp"

QMLSlider::QMLSlider(Qt::Orientation orientat, QWidget* parent)
    : QMLWidgetBase(":/QML/Slider.qml", parent)
{
    value = QQmlProperty(base(), "value");
    pressed = QQmlProperty(base(), "pressed");
    orientation = QQmlProperty(base(), "orientation");
    orientation.write(orientat);
    if (orientat == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

QQuickItem* QMLSlider::base() {
    return findItem("slider");
}
