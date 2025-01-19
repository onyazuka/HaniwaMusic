#ifndef QMLCONTROLBUTTON_HPP
#define QMLCONTROLBUTTON_HPP
#include "QMLWidgetBase.hpp"

class QMLControlButton  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLControlButton(const QString& text, QWidget* parent = nullptr);
    void setText(const QString& text);
};

#endif // QMLCONTROLBUTTON_HPP
