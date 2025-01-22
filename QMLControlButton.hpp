#ifndef QMLCONTROLBUTTON_HPP
#define QMLCONTROLBUTTON_HPP
#include "QMLWidgetBase.hpp"

class QMLControlButton  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLControlButton(const QString& iconSource, QWidget* parent = nullptr);
    QQuickItem* base() override;
    QQmlProperty icon_source;
};

#endif // QMLCONTROLBUTTON_HPP
