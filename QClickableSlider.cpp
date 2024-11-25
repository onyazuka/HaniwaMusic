#include "QClickableSlider.h"
#include <QMouseEvent>

QClickableSlider::QClickableSlider(QWidget *parent)
    : QSlider(parent)
{
    ;
}

QClickableSlider::QClickableSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
{
    ;
}

void QClickableSlider::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::MouseButton::LeftButton) {
        setSliderDown(true);
        float posRel = (float)ev->pos().x() / (float)width();
        setValue(minimum() + (maximum() - minimum()) * posRel);
        setSliderDown(false);
    }
    else {
        QSlider::mousePressEvent(ev);
    }
}
