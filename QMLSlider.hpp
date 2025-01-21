#ifndef QMLSLIDER_HPP
#define QMLSLIDER_HPP

#include "QMLWidgetBase.hpp"

class QMLSlider  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
    QQuickItem* base();
    QQmlProperty value;
    QQmlProperty pressed;
    QQmlProperty orientation;
};

#endif // QMLSLIDER_HPP
