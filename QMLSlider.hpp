#ifndef QMLSLIDER_HPP
#define QMLSLIDER_HPP

#include "QMLWidgetBase.hpp"

class QMLSlider  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
    void setOrientation(Qt::Orientation orientation);
    QQuickItem* slider();
};

#endif // QMLSLIDER_HPP
